#ifndef GUI_TEXTURE_H
#define GUI_TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture {
private:
    unsigned int rendererId;
    int width, height, BPP;
public:
    explicit Texture(const std::string &filePath);

    ~Texture();

    void Bind(unsigned int slot = 0) const;

    static void Unbind();

    [[nodiscard]] inline int GetWidth() const { return width; }

    [[nodiscard]] inline int GetHeight() const { return height; }


};


#endif //GUI_TEXTURE_H
