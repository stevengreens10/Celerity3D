#include "Texture.h"

#include "stb_image.h"
#include "../log.h"

Texture::Texture(const std::string &filePath)
        : rendererId(-1), width(0), height(0), BPP(0) {

  // Flips upside down
  stbi_set_flip_vertically_on_load(1);
  // RGBA = 4 channels
  unsigned char *localBuffer = stbi_load(filePath.c_str(), &width, &height, &BPP, 4);

  glGenTextures(1, &rendererId);
  Log::logf("Creating texture: %d", rendererId);
  glBindTexture(GL_TEXTURE_2D, rendererId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Distant texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Near texture

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
  glGenerateMipmap(GL_TEXTURE_2D);
  Unbind();

  if (localBuffer)
    stbi_image_free(localBuffer);
}

Texture::~Texture() {
  Log::logf("Deleting texture %d", rendererId);
  glDeleteTextures(1, &rendererId);
}

void Texture::Bind(unsigned int slot /* = 0 */) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, rendererId);
}

void Texture::Unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}
