#ifndef GUI_SHADER_H
#define GUI_SHADER_H

// If matrices need to be transposed
#define TRANSPOSE false

#include "../util.h"
#include "GL/glew.h"
#include "glm/detail/type_mat4x4.hpp"
#include <unordered_map>

class Shader {

private:
  unsigned int rendererId;
  std::unordered_map<char*, int> uniformCache;
public:
    Shader(char *vertFile, char *fragFile);
    ~Shader();

    void Bind() const;
    static void Unbind() ;

    // Set uniforms
    void SetUniform4f(char *name, float v0, float v1, float v2, float v3);
    void SetUniform1i(char *name, int v0);
    void SetUniform1f(char *name, float v0);

    void SetUniformMat4f(char *name, glm::mat4 mat);

private:
    int GetUniformLocation(char *name);
    static unsigned int CompileShader(char *source, GLenum type);
};


#endif //GUI_SHADER_H
