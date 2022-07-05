#include "TriangularPrism.h"

TriangularPrism::TriangularPrism(Material &m) : Renderable(std::reference_wrapper<Material>(m)) {
  vao = VAO.get();
  ibo = IBO.get();
}


std::shared_ptr<VertexArray> TriangularPrism::CreateVertexBuffer() {
  float size = 1.0f;
  float x = -0.5f, y = -0.5f, z = -0.5f;
  float vertices[] = {
          // Front vertices
          x, y, z,
          x + size, y, z,
          x + size / 2.0f, y + size, z,
          // Back vertices
          x, y, z + size,
          x + size, y, z + size,
          x + size / 2.0f, y + size, z + size
  };


  auto vao = std::make_shared<VertexArray>();
  vao->Bind();
  VertexBuffer vertBuf(vertices, sizeof(vertices));

  BufferLayout layout = BufferLayout();
  layout.Push<float>(3);
  vao->AddBuffer(vertBuf, layout);

  return vao;
}

std::shared_ptr<IndexBuffer> TriangularPrism::CreateIndexBuffer() {

  uint32_t indices[] = {
          // Front face
          0, 1, 2,
          // Right Face
          1, 4, 2,
          2, 5, 4,
          // Left Face
          0, 3, 2,
          2, 5, 3,
          // Back Face
          3, 4, 5,
          // Bottom Face
          0, 1, 4,
          4, 3, 0,
  };
  auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
  return ibo;
}

void TriangularPrism::InitBuffers() {
  VAO = CreateVertexBuffer();
  IBO = CreateIndexBuffer();
}
