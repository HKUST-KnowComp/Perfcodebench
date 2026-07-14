#include "interface.h"

#include "xxhash.h"

#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  size_t len = input.size();
  size_t pos = 0;
  while (pos < len) {
    size_t end = pos;
    while (end < len && data[end] != '\n') {
      ++end;
    }
    const uint64_t line_hash = XXH64(data + pos, end - pos, 0);
    acc ^= line_hash;
    acc *= 1099511628211ULL;
    pos = end + 1;
  }
  return acc;
}
