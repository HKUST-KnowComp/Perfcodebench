#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t fnv1a_repeat_u32(uint64_t hash, uint32_t value, uint32_t count) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (uint32_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(value);
    hash *= kPrime;
  }
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;

  if (iters <= 0) return 0;
  const std::size_t n = offsets.size();
  if (n <= 1) return kOffsetBasis;

  uint64_t hash = kOffsetBasis;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffsetBasis;

    const uint32_t* p = offsets.data();
    const uint32_t* const end = p + n - 1;
    uint32_t seg = 0;

    while (p < end) {
      const uint32_t len = p[1] - p[0];
      hash = fnv1a_repeat_u32(hash, seg, len);
      ++p;
      ++seg;
    }
  }
  return hash;
}
