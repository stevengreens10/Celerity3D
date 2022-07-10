#include "Square.h"

std::shared_ptr<VertexArray> Square::CreateVertexBuffer() {
  float size = 2;
  float x = -1, y = -1;
  Vertex vertices[] = {
          {{x,        y,        0}, {0.0f, 0.0f}},
          {{x + size, y,        0}, {1.0f, 0.0f}},
          {{x + size, y + size, 0}, {1.0f, 1.0f}},
          {{x,        y + size, 0}, {0.0f, 1.0f}},
  };


  auto vertexArray = std::make_shared<VertexArray>();
  vertexArray->Bind();
  VertexBuffer vertBuf(vertices, sizeof(vertices));

  BufferLayout layout = BufferLayout();
  layout.Push<float>(3);
  layout.Push<float>(2);
  vertexArray->AddBuffer(vertBuf, layout);

  return vertexArray;
}

std::shared_ptr<IndexBuffer> Square::CreateIndexBuffer() {

  uint32_t indices[] = {
          0, 1, 2,
          2, 3, 0,
  };
  auto idxBuffer = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
  return idxBuffer;
}

void Square::InitBuffers() {
  vao = CreateVertexBuffer();
  ibo = CreateIndexBuffer();
}

