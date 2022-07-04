#include "VertexBuffer.h"
#include "GL/glew.h"
#include <iostream>

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  unsigned int id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  rendererID = id;
  std::cout << "Creating vertex buf: " << rendererID << ", size: " << size << std::endl;
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}
