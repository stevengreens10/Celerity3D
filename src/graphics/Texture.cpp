#include "Texture.h"

#include "stb_image.h"

Texture::Texture(const char *filePath)
        : rendererId(0), width(0), height(0), BPP(0) {

  // Flips upside down
  stbi_set_flip_vertically_on_load(1);
  // RGBA = 4 channels
  unsigned char *localBuffer = stbi_load(filePath, &width, &height, &BPP, 4);

  glGenTextures(1, &rendererId);
  printf("Creating texture: %d\n", rendererId);
  glBindTexture(GL_TEXTURE_2D, rendererId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Clamp instead of tile for x and y
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
  Unbind();

  if (localBuffer)
    stbi_image_free(localBuffer);
}

Texture::~Texture() {
  printf("Deleting texture %d\n", rendererId);
  glDeleteTextures(1, &rendererId);
}

void Texture::Bind(unsigned int slot /* = 0 */) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, rendererId);
}

void Texture::Unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}
