#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  const char* line_start = p;
  while (p < end) {
    if (*p == '
') {
      acc ^= XXH64(line_start, static_cast<std::size_t>(p - line_start), 0);
      acc *= 1099511628211ULL;
      line_start = p + 1;
    }
    ++p;
  }
  if (line_start < end) {
    acc ^= XXH64(line_start, static_cast<std::size_t>(end - line_start), 0);
    acc *= 1099511628211ULL;
  }
  return acc;
}