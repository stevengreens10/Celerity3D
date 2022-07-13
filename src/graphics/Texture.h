#ifndef GUI_TEXTURE_H
#define GUI_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>

using std::unordered_map, std::string;

class Texture {
protected:
    int channels = 0, samples = 0;
    GLenum internalFormat = 0, format = 0, dataType = 0;

    inline static unordered_map<string, Texture *> textures;

    Texture() = default;

    int height = 0;
    int width = 0;
public:
    unsigned int id = -1;
    GLenum texType = 0;

    Texture(int width, int height);

    explicit Texture(int samples, int width, int height, GLenum internalFmt, GLenum fmt, GLenum type);

    ~Texture();

    static Texture *Load(const std::string &filePath, bool mipmap);

    virtual void SetPixels(unsigned char *pixels) const;

    virtual void Resize(int w, int h);

    void Bind(unsigned int slot = 0) const;

    void Unbind();

    [[nodiscard]] inline int GetWidth() const { return width; }

    [[nodiscard]] inline int GetHeight() const { return height; }


};


#endif //GUI_TEXTURE_H
