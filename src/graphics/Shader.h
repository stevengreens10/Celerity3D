#ifndef GUI_SHADER_H
#define GUI_SHADER_H

#include "../util.h"
#include "GL/glew.h"
#include "glm/detail/type_mat4x4.hpp"
#include <unordered_map>

class Shader {

public:
    unsigned int rendererId;

    Shader(const char *vertFile, const char *fragFile);

    ~Shader();

    void Bind() const;

    static void Unbind();

private:
    static unsigned int CompileShader(char *source, GLenum type);
};


#endif //GUI_SHADER_H
