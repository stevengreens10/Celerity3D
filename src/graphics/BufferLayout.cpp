#include "BufferLayout.h"

std::unordered_map<std::type_index, unsigned int> BufferLayout::glTypeMap = createMap();