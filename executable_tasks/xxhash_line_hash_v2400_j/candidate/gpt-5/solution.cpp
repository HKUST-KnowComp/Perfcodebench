#include "interface.h"
#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const void* nl = std::memchr(p, '\n', static_cast<size_t>(end - p));
    const char* q = static_cast<const char*>(nl);

    size_t len = q ? static_cast<size_t>(q - p) : static_cast<size_t>(end - p);
    uint64_t h = static_cast<uint64_t>(XXH64(p, len, 0));
    acc ^= h;
    acc *= 1099511628211ULL;

    if (!q) break; // no more newlines; processed the final segment
    p = q + 1;     // advance past the newline (handles empty lines too)
  }

  return acc;
}
