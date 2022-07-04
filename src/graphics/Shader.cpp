#include "Shader.h"

Shader::Shader(const char *vertFile, const char *fragFile) {
  char *vertSource = readFile(vertFile);
  char *fragSource = readFile(fragFile);

  if(vertSource == nullptr || fragSource == nullptr) {
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

void Shader::SetUniform1f(char *name, float v0) {
  glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform4f(char *name, float v0, float v1, float v2, float v3) {
  glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(char *name, int v0) {
  glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniformMat4f(char *name, glm::mat4 mat) {
  glUniformMatrix4fv(GetUniformLocation(name), 1, TRANSPOSE, &mat[0][0]);
}

int Shader::GetUniformLocation(char *name) {
  if(uniformCache.contains(name)) return uniformCache[name];

  int location = glGetUniformLocation(rendererId, name);
  uniformCache[name] = location;
  return location;
}


unsigned int Shader::CompileShader(char *source, GLenum type) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  return id;
}