#include "CubeTexture.h"
#include "GL/glew.h"
#include "stb_image.h"
#include "../log.h"

CubeTexture::CubeTexture(int w, int h) : CubeTexture(w, h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE) {}

CubeTexture::CubeTexture(int w, int h, GLenum internalFmt, GLenum fmt, GLenum type) {
  glGenTextures(1, &id);
  texType = GL_TEXTURE_CUBE_MAP;
  width = w;
  height = h;
  internalFormat = internalFmt;
  format = fmt;
  dataType = type;
  Bind();
  for (int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (GLint) internalFormat, width, height,
                 0, format, dataType, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  Unbind();
}

CubeTexture::~CubeTexture() {
  glDeleteTextures(1, &id);
}

CubeTexture *CubeTexture::Load(const string &basePath, const vector<string> &names) {
  auto cubeTexture = new CubeTexture(0, 0);

  cubeTexture->Bind();
  int width, height, nComponents;
  for (int i = 0; i < 6; i++) {
    unsigned char *pixelData = stbi_load((basePath + names[i]).c_str(), &width, &height, &nComponents, 3);
    if (pixelData) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                   pixelData);
      stbi_image_free(pixelData);
    } else {
      Log::logf("ERROR: Unable to load %s", (basePath + names[i]).c_str());
      return nullptr;
    }
  }

  cubeTexture->width = width;
  cubeTexture->height = height;
  return cubeTexture;
}