#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  std::size_t len = input.size();
  std::size_t i = 0;
  while (i < len) {
    std::size_t start = i;
    while (i < len && data[i] != '\n') {
      ++i;
    }
    const char* line_data = data + start;
    std::size_t line_len = i - start;
    acc ^= static_cast<uint64_t>(XXH64(line_data, line_len, 0));
    acc *= 1099511628211ULL;
    if (i < len) {
      ++i;
    }
  }
  return acc;
}