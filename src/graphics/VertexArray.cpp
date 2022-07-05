#include <iostream>
#include "VertexArray.h"
#include "BufferLayout.h"
#include "GL/glew.h"

VertexArray::VertexArray() {
  glGenVertexArrays(1, &rendererID);
  std::cout << "Creating vertex array: " << rendererID << std::endl;
}

VertexArray::~VertexArray() {
  std::cout << "Deleting vertex array: " << rendererID << std::endl;

  for (auto bufId: bufIds) {
    std::cout << "\tDeleting vertex buffer: " << bufId << std::endl;
    glDeleteBuffers(1, &bufId);
  }

  glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::AddBuffer(const VertexBuffer &buffer, const BufferLayout &layout) {
  std::cout << "Adding vertex array vbuf: " << rendererID << std::endl;
  Bind();
  buffer.Bind();
  const auto &elements = layout.GetElements();
  for (int i = 0; i < elements.size(); i++) {
    auto element = elements[i];
    glVertexAttribPointer(i, element.count, element.type, element.normalized,
                          layout.GetStride(), (void *) element.offset);
    glEnableVertexAttribArray(i);
  }
  bufIds.push_back(buffer.GetId());
}

void VertexArray::Bind() const {
  glBindVertexArray(rendererID);
}

void VertexArray::Unbind() const {
  glBindVertexArray(0);
}

