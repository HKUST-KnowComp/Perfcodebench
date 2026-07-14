#include "interface.h"

#include <string>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* const end = data + text.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const char* start = data;

    for (const char* p = data; p < end; ++p) {
      if (*p == '\n') {
        std::size_t len = static_cast<std::size_t>(p - start);
        h = mix(h, len);
        uint64_t first = (len == 0) ? 0u : static_cast<unsigned char>(*start);
        h = mix(h, first);
        start = p + 1;
      }
    }

    // Process the final line if the text does not end with a newline.
    if (start < end) {
      std::size_t len = static_cast<std::size_t>(end - start);
      h = mix(h, len);
      uint64_t first = static_cast<unsigned char>(*start);
      h = mix(h, first);
    }

    hash = h;  // behavior matches baseline: hash from the last iteration is returned
  }

  return hash;
}
