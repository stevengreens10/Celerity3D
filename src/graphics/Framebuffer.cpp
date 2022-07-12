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
    glDeleteTextures(GL_TEXTURE_2D_MULTISAMPLE, &it.second);
  }
}

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

void Framebuffer::AddTextureAttachment(GLenum type, int width, int height) {
  Bind();

  auto texFmts = getTexFormats(type);

  unsigned int texture;
  glGenTextures(1, &texture);
  textureIDs[type] = texture;
  if (samples > 1) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, texFmts.internalTexFormat, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);
  } else {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, texFmts.internalTexFormat, width, height, 0, texFmts.texFormat, texFmts.texType,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, texture, 0);
  }
}

void Framebuffer::DisableColor() {
  Bind();
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
}

unsigned int Framebuffer::GetTexture(GLenum type) {
  return textureIDs[type];
}

void Framebuffer::Resize(int width, int height) {
  if (!resizeToScreen)
    return;

  for (auto &it: textureIDs) {
    auto type = it.first;
    auto texId = it.second;
    TextureFormats texFmts = getTexFormats(type);

    if (samples > 1) {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texId);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, texFmts.internalTexFormat, width, height, GL_TRUE);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {
      glBindTexture(GL_TEXTURE_2D, texId);
      glTexImage2D(GL_TEXTURE_2D, 0, texFmts.internalTexFormat, width, height, 0, texFmts.texFormat, texFmts.texType,
                   nullptr);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}

void Framebuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::BindTexture(GLenum type) {
  if (samples > 1)
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureIDs[type]);
  else
    glBindTexture(GL_TEXTURE_2D, textureIDs[type]);
}


void Framebuffer::Unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::UnbindTexture(GLenum type) const {
  if (samples > 1)
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  else
    glBindTexture(GL_TEXTURE_2D, 0);
}
