#ifndef GUI_TEXTURE_H
#define GUI_TEXTURE_H

#include <GL/glew.h>

class Texture {
private:
    unsigned int rendererId;
    int width, height, BPP;
public:
    Texture(const char *filePath);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    static void Unbind() ;

    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }


};


#endif //GUI_TEXTURE_H
