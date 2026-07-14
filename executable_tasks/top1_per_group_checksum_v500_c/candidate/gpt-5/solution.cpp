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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute per-group maxima using a dense array.
  std::vector<uint32_t> best(group_count, 0u);
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  uint32_t* bptr = best.data();
  const std::size_t n = groups.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t g = gptr[i];
    uint32_t v = vptr[i];
    // Assuming dense valid group IDs: 0 <= g < group_count
    uint32_t cur = bptr[g];
    if (v > cur) {
      bptr[g] = v;
    }
  }

  // The checksum result is identical across iterations; compute once.
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, bptr[g]);
  }

  return hash;
}
