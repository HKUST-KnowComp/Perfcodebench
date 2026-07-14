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
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;

  // The baseline materializes the expanded stream and then hashes it.
  // We can hash the exact same logical stream directly, avoiding all
  // allocation, clearing, and push_back overhead while preserving order.
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    if (n >= 2) {
      for (std::size_t s = 0; s + 1 < n; ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t value = static_cast<uint32_t>(s);
        for (uint32_t i = 0; i < len; ++i) {
          hash = mix(hash, value);
        }
      }
    }
  }

  return hash;
}
