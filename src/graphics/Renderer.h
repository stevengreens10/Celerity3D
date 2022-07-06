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
#include "Renderable.h"
#include "Cube.h"

class Renderer {
private:
    glm::mat4 proj;
    glm::mat4 view;
public:
    static void Clear();

    void Draw(const Renderable &r) const;

    void SetProjection(int width, int height);

    void CalculateView();

    static glm::mat4 GetModel(glm::vec3 pivot, float modelScale, glm::vec3 modelTranslate, glm::vec3 modelRotate);

    static void NewFrame();

    void Init(ApplicationWindow *win);

    static void Update(ApplicationWindow *win);


    static void Cleanup(ApplicationWindow *win);

    static void DrawRenderableDebug(const std::string &name, Renderable *r);

    void RotateCamera(float _yaw, float pitch);

    glm::vec3 cameraPos;
    glm::vec3 cameraDir;
    float yaw;
    float pitch;
private:
    static void InitImGui(ApplicationWindow *win);

    static void UpdateImGui(ApplicationWindow *win);

};

#endif //GUI_RENDERER_H
