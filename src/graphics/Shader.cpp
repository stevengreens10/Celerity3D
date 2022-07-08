#include "Shader.h"
#include "../log.h"

#include <utility>

Shader::Shader(const std::string &vertFile, const std::string &fragFile) {
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

unsigned int Shader::CreateBuffer(GLenum type, const string &name, BufferLayout layout, int idx) {
  uint32_t id;
  glGenBuffers(1, &id);
  glBindBuffer(type, id);

  uint32_t size = 0;
  for (auto element: layout.GetElements()) {
    for (int i = 0; i < element.count; i++) {
      uint32_t offset = size - (size / element.alignment) * element.alignment;
      if (offset > 0)
        size += element.alignment - offset;
      size += element.size;
    }
  }

  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, idx, id);
  globalUniforms[name] = {id, size, std::move(layout)};
  return id;
}

unsigned int Shader::CreateGlobalUniform(const string &name, BufferLayout layout, int idx) {
  return CreateBuffer(GL_UNIFORM_BUFFER, name, std::move(layout), idx);
}

unsigned int Shader::CreateShaderStorageBuffer(const string &name, BufferLayout layout, int idx) {
  return CreateBuffer(GL_SHADER_STORAGE_BUFFER, name, std::move(layout), idx);
}

void Shader::SetBuffer(GLenum type, const string &name, char *data) {
  auto uniformData = globalUniforms[name];
  auto layout = uniformData.layout;
  uint32_t inOffset = 0;
  uint32_t outOffset = 0;
  glBindBuffer(type, uniformData.id);
  for (auto element: layout.GetElements()) {
    for (int i = 0; i < element.count; i++) {
      uint32_t offset = outOffset - (outOffset / element.alignment) * element.alignment;
      if (offset > 0)
        outOffset += element.alignment - offset;
      glBufferSubData(type, outOffset, element.size, (void *) (data + inOffset));
      outOffset += element.size;
      inOffset += element.size;
    }
  }
}

void Shader::SetGlobalUniform(const string &name, char *data) {
  SetBuffer(GL_UNIFORM_BUFFER, name, data);
}

void Shader::SetShaderStorageBuffer(const string &name, char *data) {
  SetBuffer(GL_SHADER_STORAGE_BUFFER, name, data);
}
