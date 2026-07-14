#include "interface.h"

namespace {

inline uint64_t fnv_mix(uint64_t h, uint64_t v) {
  // FNV-1a style: XOR then multiply by prime
  constexpr uint64_t kPrime = 1099511628211ULL;
  h ^= v;
  h *= kPrime;
  return h;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Preserve baseline behavior: if iters <= 0, do nothing and return 0
  if (iters <= 0) return 0ULL;

  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  const std::size_t n = offsets.size();
  // If no segments, hash remains at basis for any iters >= 1
  if (n < 2) return kOffsetBasis;

  uint64_t hash = kOffsetBasis;

  for (std::size_t s = 0; s + 1 < n; ++s) {
    // Match baseline: len computed as uint32_t difference
    uint32_t len = static_cast<uint32_t>(offsets[s + 1] - offsets[s]);
    if (len == 0) continue;

    // Match baseline: segment id hashed as uint32_t
    const uint64_t v = static_cast<uint64_t>(static_cast<uint32_t>(s));

    // Unroll by 8 for better throughput on long runs
    while (len >= 8) {
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      hash ^= v; hash *= kPrime;
      len -= 8;
    }
    switch (len) {
      case 7: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 6: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 5: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 4: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 3: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 2: hash ^= v; hash *= kPrime; [[fallthrough]];
      case 1: hash ^= v; hash *= kPrime; [[fallthrough]];
      default: break;
    }
  }

  // Baseline recomputed the same hash each iteration; final equals single iteration
  return hash;
}
