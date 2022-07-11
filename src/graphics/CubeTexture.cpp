#include "CubeTexture.h"
#include "GL/glew.h"
#include "stb_image.h"
#include "../log.h"

CubeTexture::CubeTexture(const string &basePath, const vector<string> &names) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);

  for (int i = 0; i < 6; i++) {
    int width, height, nComponents;
    unsigned char *pixelData = stbi_load((basePath + names[i]).c_str(), &width, &height, &nComponents, 3);
    if (pixelData) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                   pixelData);
      stbi_image_free(pixelData);
    } else {
      Log::logf("ERROR: Unable to load %s", (basePath + names[i]).c_str());
      return;
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeTexture::Bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void CubeTexture::Unbind() {
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
