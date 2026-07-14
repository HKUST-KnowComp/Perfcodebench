#include "interface.h"
#include <cstddef>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) noexcept {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const std::size_t n = offsets.size();
  if (n < 2 || iters <= 0) return 1469598103934665603ULL;

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, static_cast<uint64_t>(s));
      }
    }
  }
  return hash;
}