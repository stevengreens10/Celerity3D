#ifndef GUI_FRAMEBUFFER_H
#define GUI_FRAMEBUFFER_H

#include "GL/glew.h"
#include "Texture.h"
#include <unordered_map>

struct FramebufTexture {
    bool owned;
    Texture *tex;
};

class Framebuffer {
private:
    unsigned int id = -1;
    std::unordered_map<GLenum, FramebufTexture> textureIDs{};
public:
    int samples;
    bool resizeToScreen = true;

    Framebuffer();

    explicit Framebuffer(int numSamples);

    ~Framebuffer();

    void CreateTextureAttachment(GLenum type, int width, int height);

    void SetTextureAttachment(GLenum type, Texture *tex);

    void Resize(int width, int height);

    Texture *GetTexture(GLenum type);

    void Bind() const;

    void BindTexture(GLenum type, int slot = 0);

    void Unbind() const;

    void UnbindTexture(GLenum type);

    void DisableColor() const;

};

#endif //GUI_FRAMEBUFFER_H
