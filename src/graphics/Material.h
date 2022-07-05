#ifndef GUI_MATERIAL_H
#define GUI_MATERIAL_H

// If matrices need to be transposed
#define TRANSPOSE false

#include "Texture.h"
#include "Shader.h"

enum UniformType {
    U4f, U4i, UM4f
};

struct UniformMapping {
    UniformType type;
    void *data;
};

/**
 * Stores a shader w/ uniform values
 */
class Material {
private:
    std::unordered_map<std::string, UniformMapping> uniforms;
    std::unordered_map<std::string, int> uniformCache;
public:
    Shader &shader;
    Texture &texture;

    Material(Shader &s, Texture &t) :
            shader(s), texture(t) {}


    void SetUniform(const std::string &name, UniformType type, void *data);

    void Bind();

    void Unbind();

    int GetUniformLocation(const std::string &name);
};

#endif //GUI_MATERIAL_H
