#include "interface.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* const data = text.data();
  const std::size_t n = text.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    std::size_t start = 0;
    while (start < n) {
      std::size_t end = start;
      while (end < n && data[end] != '\n') {
        ++end;
      }

      const std::size_t len = end - start;
      h = mix(h, static_cast<uint64_t>(len));
      h = mix(h, len == 0 ? 0ULL
                          : static_cast<uint64_t>(static_cast<unsigned char>(data[start])));

      start = end + 1;
    }

    hash = h;
  }

  return hash;
}
