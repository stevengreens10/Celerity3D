#ifndef GUI_FRAMEBUFFER_H
#define GUI_FRAMEBUFFER_H

#include "GL/glew.h"
#include <unordered_map>

class Framebuffer {
private:

    unsigned int id = -1;
    std::unordered_map<GLenum, unsigned int> textureIDs;
public:
    int samples;
    bool resizeToScreen = true;

    Framebuffer();

    explicit Framebuffer(int numSamples);

    ~Framebuffer();

    void AddTextureAttachment(GLenum type, int width, int height);

    void Resize(int width, int height);

    unsigned int GetTexture(GLenum type);

    void Bind() const;

    void BindTexture(GLenum type);

    void Unbind() const;

    void UnbindTexture(GLenum type) const;

    void DisableColor();
};

#endif //GUI_FRAMEBUFFER_H
