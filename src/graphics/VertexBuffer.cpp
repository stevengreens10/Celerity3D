#include "VertexBuffer.h"
#include "GL/glew.h"
#include "../log.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  unsigned int id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  rendererID = id;
  Log::logf("Creating vertex buf: %d, size: %d", rendererID, size);
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}
