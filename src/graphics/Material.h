#ifndef GUI_MATERIAL_H
#define GUI_MATERIAL_H

// If matrices need to be transposed
#define TRANSPOSE false

#include <memory>
#include <utility>
#include "Texture.h"
#include "Shader.h"

enum UniformType {
    U4f, U3f, U4i, UM4f
};

struct UniformMapping {
    UniformType type;
    void *data;
};

struct MaterialData {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specColor;
    float shininess;
    float refractionIndex;
    float alpha;
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
    MaterialData matData;

    std::shared_ptr<Texture> ambientTex;
    std::shared_ptr<Texture> diffuseTex;
    std::shared_ptr<Texture> specTex;
    std::shared_ptr<Texture> shininessTex;

    Material(Shader &s, std::shared_ptr<Texture> diffuse) :
            shader(s), diffuseTex(std::move(diffuse)) {}


    void SetUniform(const std::string &name, UniformType type, void *data);

    void Bind();

    void Unbind();

    int GetUniformLocation(const std::string &name);

private:
    int setTexture(const std::string &name, const std::shared_ptr<Texture> &texture, int slot);

    void setMaterialData(MaterialData data);
};

#endif //GUI_MATERIAL_H