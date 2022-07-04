#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include "GL/glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {
public:
    void Clear() const;
    void Draw(const Shader &sh, const VertexArray& va, const IndexBuffer& ib) const;

    glm::mat4 GetMVP(int width, int height);

    void DrawImGui();
};

#endif //GUI_RENDERER_H
