#include "Shader.h"
#include "../log.h"

#include <utility>

Shader::Shader(const std::string &vertFile, const std::string &fragFile) {
  name = vertFile;
  char *vertSource = readFile(vertFile);
  char *fragSource = readFile(fragFile);

  if (vertSource == nullptr || fragSource == nullptr) {
    Log::logf("ERR: Sources are null");
    exit(1);
  }

  rendererId = glCreateProgram();
  uint32_t vertId = CompileShader(vertSource, GL_VERTEX_SHADER);
  uint32_t fragId = CompileShader(fragSource, GL_FRAGMENT_SHADER);
  glAttachShader(rendererId, vertId);
  glAttachShader(rendererId, fragId);
  glLinkProgram(rendererId);
  glValidateProgram(rendererId);

  // Delete intermediates
  glDeleteShader(vertId);
  glDeleteShader(fragId);
  free(vertSource);
  free(fragSource);
}

Shader::~Shader() {
  glDeleteProgram(rendererId);
}

void Shader::Bind() const {
  glUseProgram(rendererId);
}

void Shader::Unbind() {
  glUseProgram(0);
}

unsigned int Shader::CompileShader(char *source, GLenum type) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  return id;
}

Shader *Shader::LoadShader(const std::string &name) {
  return shaders[name];
}

Shader *Shader::CreateShader(const string &name) {
  auto s = new Shader(SHADER_PATH + name + ".vert", SHADER_PATH + name + ".frag");
  shaders[name] = s;
  return s;
}

uint32_t getPadding(uint32_t idx, uint32_t alignment) {
  uint32_t offset = idx - (idx / alignment) * alignment;
  if (offset > 0)
    return alignment - offset;
  return 0;
}

unsigned int Shader::CreateBuffer(GLenum type, const string &name, BufferLayout layout, int idx) {
  uint32_t id;
  glGenBuffers(1, &id);
  glBindBuffer(type, id);

  uint32_t size = 0;
  for (const auto &element: layout.GetElements()) {
    for (int i = 0; i < element.count; i++) {
      size += getPadding(size, element.alignment) + element.size;
    }
    // Account for padding at end of struct
    if (element.type == LAYOUT_TYPE)
      size += getPadding(size, element.alignment);
  }

  glBufferData(type, size, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(type, idx, id);
  globalUniforms[name] = {id, size, std::move(layout)};
  return id;
}

unsigned int Shader::CreateGlobalUniform(const string &name, BufferLayout layout, int idx) {
  return CreateBuffer(GL_UNIFORM_BUFFER, name, std::move(layout), idx);
}

unsigned int Shader::CreateShaderStorageBuffer(const string &name, BufferLayout layout, int idx) {
  return CreateBuffer(GL_SHADER_STORAGE_BUFFER, name, std::move(layout), idx);
}

void Shader::SetBuffer(GLenum type, const string &name, char *data, uint32_t bufSize) {
  auto uniformData = globalUniforms[name];
  auto layout = uniformData.layout;
  uint32_t inOffset = 0;
  uint32_t outOffset = 0;
  glBindBuffer(type, uniformData.id);
  for (const auto &element: layout.GetElements()) {
    for (int i = 0; i < element.count; i++) {
      outOffset += getPadding(outOffset, element.alignment);

      // If element is a struct/array
      // Only support one level ATM
      if (element.type == LAYOUT_TYPE) {
        for (const auto &subElement: element.subElements) {
          for (int j = 0; j < subElement.count; j++) {
            outOffset += getPadding(outOffset, subElement.alignment);

            if (inOffset > bufSize) return;
            glBufferSubData(type, outOffset, subElement.size, (void *) (data + inOffset));
            outOffset += subElement.size;
            inOffset += subElement.size;
          }
        }
        uint32_t padSize = getPadding(outOffset, element.alignment);
        // Just write garbage from the stack
        char padding[16];
        glBufferSubData(type, outOffset, padSize, (void *) &padding);
        outOffset += padSize;

      } else {
        if (inOffset > bufSize) return;
        glBufferSubData(type, outOffset, element.size, (void *) (data + inOffset));
        outOffset += element.size;
        inOffset += element.size;
      }
    }
  }
}

void Shader::SetGlobalUniform(const string &name, char *data, uint32_t bufSize) {
  SetBuffer(GL_UNIFORM_BUFFER, name, data, bufSize);
}

void Shader::SetShaderStorageBuffer(const string &name, char *data, uint32_t bufSize) {
  SetBuffer(GL_SHADER_STORAGE_BUFFER, name, data, bufSize);
}

void Shader::SetUniform(const std::string &uniName, UniformType type, void *data) {
  int location = GetUniformLocation(uniName);
  switch (type) {
    case U1f: {
      glUniform1fv(location, 1, (float *) data);
      break;
    }
    case U2f: {
      glUniform2fv(location, 1, (float *) data);
      break;
    }
    case U3f: {
      glUniform3fv(location, 1, (float *) data);
      break;
    }
    case U4f: {
      glUniform4fv(location, 1, (float *) data);
      break;
    }
    case U1i: {
      glUniform1iv(location, 1, (int *) data);
    }
    case U4i: {
      glUniform1iv(location, 1, (int *) data);
      break;
    }
    case UM4f: {
      auto *mat = (glm::mat4 *) data;
      glUniformMatrix4fv(GetUniformLocation(uniName), 1, TRANSPOSE, &((*mat)[0][0]));
      break;
    }
    default:
      std::cerr << "Unsupported uniform type." << std::endl;
  }
}

int Shader::GetUniformLocation(const std::string &uniName) {
  if (uniformCache.contains(uniName)) return uniformCache[uniName];

  int location = glGetUniformLocation(rendererId, uniName.c_str());

  if (location == -1) {
    Log::logf("WARN: Uniform %s not found", uniName.c_str());
  }

  uniformCache[uniName] = location;
  return location;
}