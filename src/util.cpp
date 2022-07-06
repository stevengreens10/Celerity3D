#include "util.h"

char *readFile(const string &path) {
  FILE *file = fopen(path.c_str(), "r");
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

vector<string> split(const string &s, const string &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  string token;
  vector<string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}
