#include <iostream>
#include "Material.h"

void Material::SetUniform(std::string name, UniformType type, void *data) {
  uniforms[name] = {type, data};
}

void Material::Bind() {
  shader.Bind();
  texture.Bind(0);
  glUniform1i(GetUniformLocation("u_Texture"), 0);
  for (const auto &uMapping: uniforms) {
    auto name = uMapping.first;
    auto u = uMapping.second;
    auto type = u.type;
    auto data = u.data;
    int location = GetUniformLocation(name);
    switch (type) {
      case U4f: {
        auto floats = (float *) data;
        glUniform4f(location, floats[0], floats[1], floats[2], floats[3]);
        break;
      }
      case U4i: {
        glUniform1i(location, (*(int *) data));
        break;
      }
      case UM4f: {
        auto *mat = (glm::mat4 *) data;
        glUniformMatrix4fv(GetUniformLocation(name), 1, TRANSPOSE, &((*mat)[0][0]));
        break;
      }
      default:
        std::cerr << "Unsupported uniform type." << std::endl;
    }

  }
}

void Material::Unbind() {

}

int Material::GetUniformLocation(std::string name) {
  if (uniformCache.contains(name)) return uniformCache[name];

  int location = glGetUniformLocation(shader.rendererId, name.c_str());

  if (location == -1) {
    printf("WARN: Uniform %s not found", name.c_str());
  }

  uniformCache[name] = location;
  return location;
}
