#include <iostream>
#include "Material.h"
#include "../log.h"

void Material::Bind() {
  shader.Bind();
  // Bitmap for textures present
  int texturesPresent = 0;
  texturesPresent |= setTexture("u_ambientTex", ambientTex, 0);
  texturesPresent |= setTexture("u_diffuseTex", diffuseTex, 1) << 1;
  texturesPresent |= setTexture("u_specTex", specTex, 2) << 2;
  texturesPresent |= setTexture("u_shinyTex", shininessTex, 3) << 3;
  texturesPresent |= setTexture("u_bumpTex", bumpTex, 4) << 4;
  shader.SetUniform("u_texturesPresent", U1i, &texturesPresent);

  setMaterialData(matData);

  for (const auto &uMapping: uniforms) {
    auto uName = uMapping.first;
    auto u = uMapping.second;
    auto type = u.type;
    auto data = u.data;
    shader.SetUniform(uName, type, data);
  }
}

int Material::setTexture(const std::string &texName, const std::shared_ptr<Texture> &texture, int slot) {
  if (texture) {
    texture->Bind(slot);
    shader.SetUniform(texName, U1i, &slot);
    return 1;
  }
  return 0;
}

void Material::setMaterialData(MaterialData data) {
  shader.SetUniform("u_ambientColor", U3f, &(data.ambientColor.x));
  shader.SetUniform("u_diffuseColor", U3f, &(data.diffuseColor.x));
  shader.SetUniform("u_specColor", U3f, &(data.specColor.x));
  shader.SetUniform("u_shininess", U1f, &(data.shininess));
  shader.SetUniform("u_refractionIndex", U1f, &(data.refractionIndex));
  shader.SetUniform("u_alpha", U1f, &(data.alpha));
}

void Material::SetUniform(const std::string &uName, UniformType type, void *data) {
  uniforms[uName] = {type, data};
}


void Material::Unbind() {

}