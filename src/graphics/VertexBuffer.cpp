#include "VertexBuffer.h"
#include "GL/glew.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  unsigned int id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  rendererID = id;
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &rendererID);
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}
