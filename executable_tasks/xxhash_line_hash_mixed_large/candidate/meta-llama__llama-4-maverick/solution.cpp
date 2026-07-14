#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* end = p + input.size();
  while (p < end) {
    const char* line_end = static_cast<const char*>(memchr(p, '
', end - p));
    size_t line_len = (line_end != nullptr) ? (line_end - p) : (end - p);
    acc ^= XXH64(p, line_len, 0);
    acc *= 1099511628211ULL;
    p += line_len + ((line_end != nullptr) ? 1 : 0);
  }
  return acc;
}