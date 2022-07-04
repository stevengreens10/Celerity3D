#include "IndexBuffer.h"
#include "GL/glew.h"
#include <iostream>

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
  : count(count) {
  glGenBuffers(1, &rendererID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
  std::cout << "Creating index buf: " << rendererID << ", count: " << count << std::endl;
}

IndexBuffer::~IndexBuffer() {
  std::cout << "Deleting index buf: " << rendererID << std::endl;
  glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
