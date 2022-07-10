#include "Framebuffer.h"
#include "../log.h"
#include "../Application.h"

struct TextureFormats {
    GLint internalTexFormat;
    GLenum texFormat;
    GLenum texType;
};

TextureFormats getTexFormats(GLenum attachType) {
  GLint internalTexFormat;
  GLenum texFormat;
  GLenum texType;

  if (attachType >= GL_COLOR_ATTACHMENT0 && attachType <= GL_COLOR_ATTACHMENT15) {
    internalTexFormat = GL_RGB;
    texFormat = GL_RGB;
    texType = GL_UNSIGNED_BYTE;
  } else if (attachType == GL_DEPTH_ATTACHMENT) {
    internalTexFormat = GL_DEPTH_COMPONENT;
    texFormat = GL_DEPTH_COMPONENT;
    texType = GL_UNSIGNED_BYTE; // ?
  } else if (attachType == GL_STENCIL_ATTACHMENT) {
    internalTexFormat = GL_STENCIL_INDEX;
    texFormat = GL_STENCIL_INDEX;
    texType = GL_UNSIGNED_BYTE; // ?
  } else if (attachType == GL_DEPTH_STENCIL_ATTACHMENT) {
    internalTexFormat = GL_DEPTH24_STENCIL8;
    texFormat = GL_DEPTH_STENCIL;
    texType = GL_UNSIGNED_INT_24_8;
  } else {
    Log::logf("ERROR: Unknown framebuffer attachType: %d", attachType);
  }
  return {internalTexFormat, texFormat, texType};
}

Framebuffer::Framebuffer() {
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &id);
  for (const auto &it: textureIDs) {
    glDeleteTextures(GL_TEXTURE_2D, &it.second);
  }
}

void Framebuffer::AddTextureAttachment(GLenum type, int width, int height) {
  Bind();

  auto texFmts = getTexFormats(type);

  unsigned int texture;
  glGenTextures(1, &texture);
  textureIDs[type] = texture;
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, texFmts.internalTexFormat, width, height, 0,
               texFmts.texFormat, texFmts.texType, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, texture, 0);

}

unsigned int Framebuffer::GetTexture(GLenum type) {
  return textureIDs[type];
}

void Framebuffer::Resize(int width, int height) {
  for (auto &it: textureIDs) {
    auto type = it.first;
    auto texId = it.second;
    glBindTexture(GL_TEXTURE_2D, texId);
    TextureFormats texFmts = getTexFormats(type);

    glTexImage2D(GL_TEXTURE_2D, 0, texFmts.internalTexFormat, width, height,
                 0,
                 texFmts.texFormat, texFmts.texType, nullptr);

  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::Unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}