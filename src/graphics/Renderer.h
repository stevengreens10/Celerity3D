#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include "GL/glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "../window.h"
#include "object/primitive/Cube.h"
#include "object/primitive/Square.h"
#include "../World.h"
#include "Framebuffer.h"

#define MAX_LIGHTS 100
#define MAX_SHADOW_LIGHTS 5

class Renderer {
private:
    inline static const int SHADOW_WIDTH = 1024;
    inline static const int SHADOW_HEIGHT = 1024;
    std::unique_ptr<Framebuffer> renderBuf;
    std::unique_ptr<Framebuffer> shadowBuf;

    std::unique_ptr<Object> skyCube;
    GLuint cubeTexArrId;
    std::unique_ptr<Square> screen;
    glm::mat4 lightProj;
    glm::mat4 lightOrtho;
    float lightFarPlane;
public:
    static void Clear();

    void Draw(const World &s) const;

    void SetProjection(int width, int height);

    void NewFrame();

    void Init(Window *win);

    void EndFrame(Window *win);


    static void Cleanup(Window *win);

    glm::mat4 proj;
private:
    static void InitImGui(Window *win);

    static void UpdateImGui(Window *win);
};

#endif //GUI_RENDERER_H
