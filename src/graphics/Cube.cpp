#include "Cube.h"

Cube::Cube(Material &m) : Renderable(std::reference_wrapper<Material>(m)) {
  pos = glm::vec3(0.0f);
  scale = 1.0f;
  rot = glm::vec3(0.0f);

  vao = CUBE_VAO.get();
  ibo = CUBE_IBO.get();
  material = m;
}


std::shared_ptr<VertexArray> Cube::CreateVertexBuffer() {
  float size = 1;
  float x = -0.5, y = -0.5, z = -0.5;
  Vertex vertices[] = {
          // Front vertices
          {{x,        y,        z},        {0.0f, 0.0f}},
          {{x + size, y,        z},        {1.0f, 0.0f}},
          {{x + size, y + size, z},        {1.0f, 1.0f}},
          {{x,        y + size, z},        {0.0f, 1.0f}},
          // Back vertices
          {{x,        y,        z + size}, {1.0f, 0.0f}},
          {{x + size, y,        z + size}, {0.0f, 0.0f}},
          {{x + size, y + size, z + size}, {0.0f, 1.0f}},
          {{x,        y + size, z + size}, {1.0f, 1.0f}}
  };


  auto vao = std::make_shared<VertexArray>();
  vao->Bind();
  VertexBuffer vertBuf(vertices, sizeof(vertices));

  BufferLayout layout = BufferLayout();
  layout.Push<float>(3);
  layout.Push<float>(2);
  vao->AddBuffer(vertBuf, layout);

  return vao;
}

std::shared_ptr<IndexBuffer> Cube::CreateIndexBuffer() {

  uint32_t indices[] = {
          // Front face
          0, 1, 2,
          2, 3, 0,
          // Right Face
          1, 5, 2,
          2, 6, 5,
          // Left Face
          0, 4, 3,
          3, 7, 4,
          // Back Face
          4, 5, 6,
          6, 7, 4,
          // Top Face
          3, 2, 6,
          6, 7, 3,
          // Bottom Face
          0, 1, 5,
          5, 4, 0,
  };
  auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
  return ibo;
}

void Cube::InitBuffers() {
  CUBE_VAO = CreateVertexBuffer();
  CUBE_IBO = CreateIndexBuffer();
}
