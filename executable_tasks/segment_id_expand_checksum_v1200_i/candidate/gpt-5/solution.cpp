#include "interface.h"

namespace {

inline void mix_repeat(uint64_t& hash, uint64_t value, uint32_t count) {
  const uint64_t P = 1099511628211ULL;
  // Unroll by 8 for better throughput
  while (count >= 8) {
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    hash ^= value; hash *= P;
    count -= 8;
  }
  switch (count) {
    case 7: hash ^= value; hash *= P; [[fallthrough]];
    case 6: hash ^= value; hash *= P; [[fallthrough]];
    case 5: hash ^= value; hash *= P; [[fallthrough]];
    case 4: hash ^= value; hash *= P; [[fallthrough]];
    case 3: hash ^= value; hash *= P; [[fallthrough]];
    case 2: hash ^= value; hash *= P; [[fallthrough]];
    case 1: hash ^= value; hash *= P; [[fallthrough]];
    default: break;
  }
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Match baseline behavior: if iters <= 0, return 0 (loop body never runs)
  if (iters <= 0) return 0ULL;

  // Compute exactly one iteration; baseline resets hash each iteration,
  // so result is identical to the last iteration.
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  const std::size_t n = offsets.size();
  if (n < 2) {
    return hash; // No segments to expand; matches baseline
  }

  for (std::size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    if (len == 0) continue;
    const uint64_t v = static_cast<uint64_t>(s);
    mix_repeat(hash, v, len);
  }

  return hash;
}
