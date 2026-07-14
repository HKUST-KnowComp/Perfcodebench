#include "interface.h"

#include <cstring>
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL; // FNV offset basis
  const uint64_t prime = 1099511628211ULL; // FNV prime

  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const size_t remaining = static_cast<size_t>(end - p);
    const char* const nl = static_cast<const char*>(std::memchr(p, '\n', remaining));
    const size_t len = nl ? static_cast<size_t>(nl - p) : remaining;

    uint64_t h;
    if (len == 0) {
      // XXH64 of empty input with seed 0
      h = 0xEF46DB3751D8E999ULL;
    } else {
      h = XXH64(p, len, 0);
    }

    acc ^= h;
    acc *= prime;

    if (!nl) break; // no more newlines; last line processed
    p = nl + 1;     // advance past newline
  }

  return acc;
}
