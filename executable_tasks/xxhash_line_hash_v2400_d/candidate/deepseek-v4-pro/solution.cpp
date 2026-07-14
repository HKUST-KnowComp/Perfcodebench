#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const size_t len = input.size();
  if (len == 0) return acc;
  size_t start = 0;
  const char* data = input.data();
  while (start < len) {
    size_t end = input.find('\n', start);
    if (end == std::string::npos) end = len;
    size_t line_len = end - start;
    acc ^= static_cast<uint64_t>(XXH64(data + start, line_len, 0));
    acc *= 1099511628211ULL;
    start = end + 1;
  }
  return acc;
}