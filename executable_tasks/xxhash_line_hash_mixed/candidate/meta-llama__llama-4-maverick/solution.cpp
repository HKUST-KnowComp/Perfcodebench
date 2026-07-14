#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* end = p + input.size();
  const char* line_start = p;
  while (p < end) {
    if (*p++ == '
') {
      acc ^= XXH64(line_start, static_cast<size_t>(p - line_start - 1), 0);
      acc *= 1099511628211ULL;
      line_start = p;
    }
  }
  if (p != line_start) {
    acc ^= XXH64(line_start, static_cast<size_t>(p - line_start), 0);
    acc *= 1099511628211ULL;
  }
  return acc;
}