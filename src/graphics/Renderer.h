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
#include "Scene.h"

#define MAX_LIGHTS 100

class Renderer {
private:
    glm::mat4 proj;
public:
    static void Clear();

    void Draw(const Scene &s) const;

    void SetProjection(int width, int height);

    static glm::mat4 GetModel(glm::vec3 pivot, float modelScale, glm::vec3 modelTranslate, glm::vec3 modelRotate);

    static void NewFrame();

    void Init(Window *win);

    static void EndFrame(Window *win);


    static void Cleanup(Window *win);

private:
    static void InitImGui(Window *win);

    static void UpdateImGui(Window *win);

};

#endif //GUI_RENDERER_H
