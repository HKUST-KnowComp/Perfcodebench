#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  size_t len = input.size();
  size_t pos = 0;
  while (pos < len) {
    const char* start = data + pos;
    const char* newline = (const char*)memchr(start, '\n', len - pos);
    size_t line_len;
    if (newline) {
      line_len = newline - start;
      pos = newline - data + 1;
    } else {
      line_len = len - pos;
      pos = len;
    }
    acc ^= XXH64(start, line_len, 0);
    acc *= 1099511628211ULL;
  }
  return acc;
}