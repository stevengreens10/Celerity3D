#ifndef GUI_MATERIAL_H
#define GUI_MATERIAL_H

#include <memory>
#include <utility>
#include "Texture.h"
#include "Shader.h"


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
public:
    Shader &shader;
    MaterialData matData{};

    std::string name;
    std::shared_ptr<Texture> ambientTex;
    std::shared_ptr<Texture> diffuseTex;
    std::shared_ptr<Texture> specTex;
    std::shared_ptr<Texture> shininessTex;
    std::shared_ptr<Texture> bumpTex;

    Material(Shader &s, std::string name) :
            shader(s), name(std::move(name)) {}


    void SetUniform(const std::string &uName, UniformType type, void *data);

    void Bind();

    void Unbind();

    int GetUniformLocation(const std::string &uniName);

private:
    int setTexture(const std::string &texName, const std::shared_ptr<Texture> &texture, int slot);

    void setMaterialData(MaterialData data);
};

#endif //GUI_MATERIAL_H
