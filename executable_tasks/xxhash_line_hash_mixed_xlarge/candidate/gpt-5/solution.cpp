#include "interface.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();

  uint64_t acc = 1469598103934665603ULL; // FNV offset basis

  while (p < end) {
    const void* found = std::memchr(p, '\n', static_cast<size_t>(end - p));
    const char* line_end = found ? static_cast<const char*>(found) : end;

    const size_t len = static_cast<size_t>(line_end - p);
    const uint64_t h = XXH64(p, len, 0);
    acc ^= h;
    acc *= 1099511628211ULL; // FNV prime

    if (!found) break;
    p = line_end + 1;
  }

  return acc;
}
