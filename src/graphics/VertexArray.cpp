#include <iostream>
#include "VertexArray.h"
#include "BufferLayout.h"
#include "GL/glew.h"

VertexArray::VertexArray() {
  glGenVertexArrays(1, &rendererID);
  Log::logf("Creating vertex array: %d", rendererID);
}

VertexArray::~VertexArray() {
  Log::logf("Deleting vertex array: ", rendererID);
  for (auto bufId: bufIds) {
    Log::logf("\tDeleting vertex buffer: %d", bufId);
    glDeleteBuffers(1, &bufId);
  }

  glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::AddBuffer(const VertexBuffer &buffer, const BufferLayout &layout) {
  Log::logf("Adding vertex array vbuf: %d", rendererID);
  Bind();
  buffer.Bind();
  const auto &elements = layout.GetElements();
  for (int i = 0; i < elements.size(); i++) {
    auto element = elements[i];
    glVertexAttribPointer(i, (GLint) element.count, element.type, element.normalized,
                          (GLsizei) layout.GetStride(), reinterpret_cast<const void *>(element.offset));
    glEnableVertexAttribArray(i);
  }
  bufIds.push_back(buffer.GetId());
}

void VertexArray::Bind() const {
  glBindVertexArray(rendererID);
}

void VertexArray::Unbind() {
  glBindVertexArray(0);
}

