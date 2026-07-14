#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  std::size_t size = input.size();
  std::size_t pos = 0;
  while (pos < size) {
    std::size_t line_len = 0;
    while (pos + line_len < size && data[pos + line_len] != '
') {
      ++line_len;
    }
    acc ^= XXH64(data + pos, line_len, 0);
    acc *= 1099511628211ULL;
    pos += line_len + 1;
  }
  return acc;
}