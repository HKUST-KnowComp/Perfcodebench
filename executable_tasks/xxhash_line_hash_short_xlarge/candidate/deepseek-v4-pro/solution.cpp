#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
    size_t length;
    if (line_end) {
      length = line_end - p;
    } else {
      length = end - p;
    }
    acc ^= static_cast<uint64_t>(XXH64(p, length, 0));
    acc *= 1099511628211ULL;
    if (line_end) {
      p = line_end + 1;
    } else {
      p = end;
    }
  }
  return acc;
}
