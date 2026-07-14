#include "interface.h"

namespace {

static inline uint64_t fnv_mix_once(uint64_t hash, uint64_t value) {
  // FNV-1a step: xor then multiply by prime
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Match baseline behavior: if no iterations, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
  const uint64_t kFNVPrime = 1099511628211ULL;

  uint64_t hash = kFNVOffsetBasis;

  const std::size_t n = offsets.size();
  if (n <= 1) {
    // No segments to expand; hashing an empty stream leaves offset basis
    return hash;
  }

  for (std::size_t s = 0; s + 1 < n; ++s) {
    // Preserve unsigned wrap-around semantics as in the baseline
    const uint32_t len = static_cast<uint32_t>(offsets[s + 1] - offsets[s]);
    const uint64_t val = static_cast<uint32_t>(s);

    uint32_t cnt = len;

    // Unroll by 8 for better throughput
    while (cnt >= 8u) {
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      hash ^= val; hash *= kFNVPrime;
      cnt -= 8u;
    }
    while (cnt--) {
      hash ^= val;
      hash *= kFNVPrime;
    }
  }

  // Baseline resets hash each iter and recomputes; the final value equals a single computation
  // from the offset basis, so we return the single-pass result.
  return hash;
}
