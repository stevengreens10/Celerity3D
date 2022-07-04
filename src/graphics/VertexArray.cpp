#include <iostream>
#include "VertexArray.h"
#include "BufferLayout.h"
#include "GL/glew.h"

VertexArray::VertexArray() {
  glGenVertexArrays(1, &rendererID);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& buffer, const BufferLayout &layout) {
  Bind();
  buffer.Bind();
  const auto& elements = layout.GetElements();
  for(int i = 0; i < elements.size(); i++) {
    auto element = elements[i];
    glVertexAttribPointer(i, element.count, element.type, element.normalized,
                          layout.GetStride(), (void *)element.offset);
    glEnableVertexAttribArray(i);
  }

}

void VertexArray::Bind() const {
  glBindVertexArray(rendererID);
}

void VertexArray::Unbind() const {
  glBindVertexArray(0);
}

