#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0;
  }
  const std::size_t n = offsets.size();
  if (n < 2) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* data = offsets.data();
  for (std::size_t s = 0; s < n - 1; ++s) {
    const uint32_t len = data[s + 1] - data[s];
    const uint32_t seg_id = static_cast<uint32_t>(s);
    for (uint32_t i = 0; i < len; ++i) {
      hash = mix(hash, seg_id);
    }
  }
  return hash;
}