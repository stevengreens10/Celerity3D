#ifndef GUI_SHADER_H
#define GUI_SHADER_H

#include "../util.h"
#include "GL/glew.h"
#include "glm/detail/type_mat4x4.hpp"
#include <unordered_map>

class Shader {

private:
    inline static std::unordered_map<std::string, Shader *> shaders;
    inline static std::string SHADER_PATH = "assets/shaders/";
public:
    unsigned int rendererId;

    ~Shader();

    void Bind() const;

    static void Unbind();

    static Shader *LoadShader(const std::string &name);

    static Shader *CreateShader(const std::string &name);

private:
    Shader(const std::string &vertFile, const std::string &fragFile);

    static unsigned int CompileShader(char *source, GLenum type);
};


#endif //GUI_SHADER_H
