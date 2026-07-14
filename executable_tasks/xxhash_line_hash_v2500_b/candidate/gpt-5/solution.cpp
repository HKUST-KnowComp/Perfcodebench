#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  const char* data = input.data();
  const size_t size = input.size();
  uint64_t acc = 1469598103934665603ULL;

  const char* end = data + size;
  const char* line_start = data;
  const char* p = data;

  while (p != end) {
    if (*p == '\n') {
      const size_t len = static_cast<size_t>(p - line_start);
      acc ^= static_cast<uint64_t>(XXH64(line_start, len, 0));
      acc *= 1099511628211ULL;
      line_start = p + 1;
    }
    ++p;
  }

  // Hash the final line segment if the input does not end with a newline
  if (line_start < end) {
    const size_t len = static_cast<size_t>(end - line_start);
    acc ^= static_cast<uint64_t>(XXH64(line_start, len, 0));
    acc *= 1099511628211ULL;
  }

  return acc;
}
