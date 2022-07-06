#include "Shader.h"

Shader::Shader(const std::string &vertFile, const std::string &fragFile) {
  char *vertSource = readFile(vertFile);
  char *fragSource = readFile(fragFile);

  if (vertSource == nullptr || fragSource == nullptr) {
    printf("ERR: Sources are null\n");
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
