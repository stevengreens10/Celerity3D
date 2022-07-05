#include "util.h"

char *readFile(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    return nullptr;
  }
  // Get file size
  fseek(file, 0L, SEEK_END);
  long sz = ftell(file);
  rewind(file);

  char *contents = (char *) malloc(sz + 1);
  int idx = 0;

  int ch;
  while ((ch = fgetc(file)) != EOF) {
    contents[idx++] = (char) ch;
  }
  contents[idx] = '\0';


  return contents;
}