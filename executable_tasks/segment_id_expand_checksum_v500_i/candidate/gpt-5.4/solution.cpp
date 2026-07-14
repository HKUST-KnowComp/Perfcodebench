#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const std::size_t n = offsets.size();
  uint64_t hash = 0;

  if (n < 2 || iters <= 0) {
    return hash;
  }

  // The baseline materializes the expanded stream and then hashes it.
  // Since the checksum depends only on the expanded values in order,
  // we can hash directly while iterating segment lengths, avoiding all
  // allocation, clearing, and push_back overhead.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t value = static_cast<uint32_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        h = mix(h, value);
      }
    }

    hash = h;
  }

  return hash;
}
