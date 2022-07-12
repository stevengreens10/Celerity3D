#include "Texture.h"

#include "stb_image.h"
#include "../log.h"

Texture::Texture(int width, int height) : Texture(0, width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT) {}

Texture::Texture(int numSamples, int w, int h, GLenum internalFmt, GLenum fmt, GLenum type) :
        samples(numSamples), width(w), height(h), internalFormat(internalFmt),
        format(fmt), dataType(type) {
  glGenTextures(1, &id);
  Log::logf("Creating texture: %d", id);
  texType = GL_TEXTURE_2D;
  if (samples > 1) {
    texType = GL_TEXTURE_2D_MULTISAMPLE;
    Bind();
    glTexImage2DMultisample(texType, samples, internalFormat, width, height, GL_TRUE);
  } else {
    Bind();
    glTexImage2D(texType, 0, (GLint) internalFormat, width, height, 0, format, dataType,
                 nullptr);
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  }
  Unbind();
}

Texture::~Texture() {
  Log::logf("Deleting texture %d", id);
  glDeleteTextures(1, &id);
}

Texture *Texture::Load(const std::string &filePath, bool mipmap) {
  if (textures.contains(filePath))
    return textures[filePath];

  // Flips upside down
  stbi_set_flip_vertically_on_load(1);
  // RGBA = 4 channels
  int width, height, channels;
  unsigned char *localBuffer = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

  auto *t = new Texture(width, height);
  t->Bind();
  GLint minFilter = GL_LINEAR;
  if (mipmap)
    minFilter = GL_LINEAR_MIPMAP_LINEAR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); // Distant texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Near texture

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
//  t->SetPixels(localBuffer);
  if (mipmap)
    glGenerateMipmap(GL_TEXTURE_2D);
  t->Unbind();

  if (localBuffer)
    stbi_image_free(localBuffer);

  textures[filePath] = t;
  return t;
}

void Texture::Resize(int w, int h) {
  width = w;
  height = h;

  Bind();
  if (texType == GL_TEXTURE_2D_MULTISAMPLE) {
    glTexImage2DMultisample(texType, samples, internalFormat, width, height, GL_TRUE);
  } else {
    glTexImage2D(texType, 0, (GLint) internalFormat, width, height, 0, format, dataType, nullptr);
  }
  Unbind();

}

void Texture::SetPixels(unsigned char *pixels) const {
  glTexImage2D(texType, 0, (GLint) internalFormat, width, height, 0, format, dataType, pixels);
}

void Texture::Bind(unsigned int slot /* = 0 */) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(texType, id);
}

void Texture::Unbind() {
  glBindTexture(texType, 0);
}
