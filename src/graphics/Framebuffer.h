#ifndef GUI_FRAMEBUFFER_H
#define GUI_FRAMEBUFFER_H

#include "GL/glew.h"
#include <unordered_map>

class Framebuffer {
private:
    unsigned int id = -1;
    std::unordered_map<GLenum, unsigned int> textureIDs;
public:
    explicit Framebuffer();

    ~Framebuffer();

    void AddTextureAttachment(GLenum type, int width, int height);

    void Resize(int width, int height);

    unsigned int GetTexture(GLenum type);

    void Bind() const;

    void Unbind() const;

};

#endif //GUI_FRAMEBUFFER_H
