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
    hash = 1469598103934665603ULL;

    std::size_t start = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (data[i] == '\n') {
        const std::size_t len = i - start;
        hash = mix(hash, static_cast<uint64_t>(len));
        hash = mix(hash, len == 0 ? 0ULL
                                  : static_cast<uint64_t>(static_cast<unsigned char>(data[start])));
        start = i + 1;
      }
    }

    if (start < n) {
      const std::size_t len = n - start;
      hash = mix(hash, static_cast<uint64_t>(len));
      hash = mix(hash, static_cast<uint64_t>(static_cast<unsigned char>(data[start])));
    }
  }

  return hash;
}
