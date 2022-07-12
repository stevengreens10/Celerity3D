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
    static Material DEFAULT;
    MaterialData matData{};

    std::string name;
    Texture *ambientTex = nullptr;
    Texture *diffuseTex = nullptr;
    Texture *specTex = nullptr;
    Texture *shininessTex = nullptr;
    Texture *bumpTex = nullptr;

    explicit Material(std::string name);


    void SetUniform(const std::string &uName, UniformType type, void *data);

    void Bind(Shader &shader);

    void Unbind();

private:
    static int
    setTexture(Shader &shader, const std::string &texName, const Texture *texture, int slot);

    static void setMaterialData(Shader &shader, MaterialData data);
};

#endif //GUI_MATERIAL_H
