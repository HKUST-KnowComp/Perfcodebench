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
  if (iters <= 0) return 0;

  constexpr uint64_t kInit = 1469598103934665603ULL;
  uint64_t hash = 0;

  const std::size_t n = offsets.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = kInit;
    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t value = static_cast<uint64_t>(static_cast<uint32_t>(s));
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, value);
      }
    }
  }

  return hash;
}
