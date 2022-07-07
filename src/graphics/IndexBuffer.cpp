#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "IndexBuffer.h"
#include "GL/glew.h"
#include "../log.h"
#include <iostream>

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
        : count(count) {
  glGenBuffers(1, &rendererID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
  Log::logf("Creating index buf: %d, count: %d", rendererID, count);
}

IndexBuffer::~IndexBuffer() {
  Log::logf("Deleting index buf: %d", rendererID);
  glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void IndexBuffer::Unbind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#pragma clang diagnostic pop