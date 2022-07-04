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

class Renderer {
private:
    glm::mat4 proj;
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
public:
    inline static std::unique_ptr<Shader> MAIN_SHADER;

    void Clear() const;
    void Draw(const Renderable &r) const;

    void SetProjection(int width, int height);
    glm::mat4 GetView() const;
    glm::mat4 GetModel(glm::vec3 pivot, float modelScale, glm::vec3 modelTranslate, glm::vec3 modelRotate) const;

    void NewFrame();

    void Init(ApplicationWindow *win);

    void Update(ApplicationWindow *win);


private:
    void InitImGui(ApplicationWindow *win);
    void UpdateImGui(ApplicationWindow *win);
    static std::unique_ptr<Shader> InitMainShader();
};

#endif //GUI_RENDERER_H
