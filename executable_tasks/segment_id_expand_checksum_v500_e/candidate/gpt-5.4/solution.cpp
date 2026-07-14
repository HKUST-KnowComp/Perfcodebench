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
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;

  const std::size_t n = offsets.size();
  if (iters <= 0) return 0;
  if (n < 2) return kOffsetBasis;

  uint64_t hash = kOffsetBasis;
  for (std::size_t s = 0; s + 1 < n; ++s) {
    uint32_t len = offsets[s + 1] - offsets[s];
    const uint64_t value = static_cast<uint32_t>(s);
    while (len >= 8) {
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      hash = mix(hash, value);
      len -= 8;
    }
    while (len--) {
      hash = mix(hash, value);
    }
  }

  return (iters & 1) ? hash : kOffsetBasis;
}
