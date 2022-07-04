#include "BufferLayout.h"


template<>
void BufferLayout::Push<float>(unsigned int count) {
  elements.push_back({GL_FLOAT, count, stride, false});
  stride += sizeof(float) * count;
}

template<>
void BufferLayout::Push<unsigned int>(unsigned int count) {
  elements.push_back({GL_UNSIGNED_INT, count, stride, true});
  stride += sizeof(unsigned int) * count;
}

template<>
void BufferLayout::Push<unsigned char>(unsigned int count) {
  elements.push_back({GL_UNSIGNED_BYTE, count, stride, true});
  stride += sizeof(unsigned char) * count;
}
