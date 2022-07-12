#ifndef GUI_CUBETEXTURE_H
#define GUI_CUBETEXTURE_H

#include <string>
#include <vector>

using std::string, std::vector;

class CubeTexture {
private:
    CubeTexture();

public:
    unsigned int id = -1;


    ~CubeTexture();

    /**
     * @param basePath Base path to the cube face textures
     * @param names The 6 filenames for the face textures in order (RIGHT, LEFT, TOP, BOTTOM, BACK, FRONT)
     */
    static CubeTexture *Load(const string &basePath, const vector<string> &names);

    static CubeTexture *Create(int width, int height);

    void Bind(unsigned int slot = 0) const;

    static void Unbind();
};


#endif //GUI_CUBETEXTURE_H
