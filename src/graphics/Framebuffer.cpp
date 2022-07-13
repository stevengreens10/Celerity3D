#include "Framebuffer.h"
#include "../log.h"
#include "../Application.h"

Framebuffer::Framebuffer() : Framebuffer(0) {}

Framebuffer::Framebuffer(int numSamples) {
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);
  samples = numSamples;
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &id);
  for (const auto &it: textureIDs) {
    auto framebufTex = it.second;
    if (framebufTex.owned)
      delete framebufTex.tex;
  }
}

struct TextureFormats {
    GLint internalFormat;
    GLenum format;
    GLenum dataType;
};

TextureFormats getTexFormats(GLenum attachType) {
  GLint internalTexFormat;
  GLenum texFormat;
  GLenum texType;

  if (attachType >= GL_COLOR_ATTACHMENT0 && attachType <= GL_COLOR_ATTACHMENT15) {
    internalTexFormat = GL_RGBA;
    texFormat = GL_RGBA;
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

void Framebuffer::CreateTextureAttachment(GLenum type, int width, int height) {
  Bind();

  auto texFmts = getTexFormats(type);
  auto *texture = new Texture(samples, width, height, texFmts.internalFormat, texFmts.format, texFmts.dataType);
  textureIDs[type] = {true, texture};

  glFramebufferTexture2D(GL_FRAMEBUFFER, type, texture->texType, texture->id, 0);
}

void Framebuffer::SetTextureAttachment(GLenum type, Texture *tex) {
  Bind();
  textureIDs[type] = {false, tex};
  if (tex->texType == GL_TEXTURE_CUBE_MAP)
    glFramebufferTexture(GL_FRAMEBUFFER, type, tex->id, 0);
  else
    glFramebufferTexture2D(GL_FRAMEBUFFER, type, tex->texType, tex->id, 0);
}

void Framebuffer::DisableColor() const {
  Bind();
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
}

Texture *Framebuffer::GetTexture(GLenum type) {
  return textureIDs[type].tex;
}

void Framebuffer::Resize(int width, int height) {
  if (!resizeToScreen)
    return;

  for (auto &it: textureIDs) {
    auto tex = it.second.tex;
    tex->Resize(width, height);
  }
}

void Framebuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::Unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BindTexture(GLenum type, int slot) {
  textureIDs[type].tex->Bind(slot);
}

void Framebuffer::UnbindTexture(GLenum type) {
  textureIDs[type].tex->Unbind();
}
