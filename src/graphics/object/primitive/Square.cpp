#include "Square.h"

std::shared_ptr<VertexArray> Square::CreateVertexBuffer() {
  Vertex2D vertices[] = {
          {{-1.0f, 1.0f},  {0.0f, 1.0f}},
          {{-1.0f, -1.0f}, {0.0f, 0.0f}},
          {{1.0f,  1.0f},  {1.0f, 1.0f}},
          {{1.0f,  -1.0f}, {1.0f, 0.0f}}
  };

  numVertices = 4;
  auto vertexArray = std::make_shared<VertexArray>();
  vertexArray->Bind();
  VertexBuffer vertBuf(vertices, sizeof(vertices));

  BufferLayout layout = BufferLayout();
  layout.Push<float>(2);
  layout.Push<float>(2);
  vertexArray->AddBuffer(vertBuf, layout);

  return vertexArray;
}

void Square::Draw(Shader &shader) {
  vao->Bind();
  material.get().Bind(shader);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);
}

void Square::InitBuffers() {
  vao = CreateVertexBuffer();
}

