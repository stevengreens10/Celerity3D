#ifndef GUI_CUBETEXTURE_H
#define GUI_CUBETEXTURE_H

#include <string>
#include <vector>
#include "Texture.h"
#include "../log.h"

using std::string, std::vector;

class CubeTexture : public Texture {
public:
    CubeTexture(int width, int height);

    CubeTexture(int width, int height, GLenum internalFmt, GLenum fmt, GLenum type);

    ~CubeTexture();

    /**
     * @param basePath Base path to the cube face textures
     * @param names The 6 filenames for the face textures in order (RIGHT, LEFT, TOP, BOTTOM, BACK, FRONT)
     */
    static CubeTexture *Load(const string &basePath, const vector<string> &names);

    void SetPixels(unsigned char *pixels) const override {
      Log::logf("ERROR: SetPixels not implemented for CubeTexture");
    };

    void Resize(int w, int h) override {
      Log::logf("ERROR: Resize not implemented for CubeTexture");
    };

};


#endif //GUI_CUBETEXTURE_H
