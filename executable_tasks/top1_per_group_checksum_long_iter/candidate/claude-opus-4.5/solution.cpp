#include "interface.h"

#include <cstring>
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
  std::vector<uint32_t> best(group_count);
  const std::size_t n = groups.size();
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  uint32_t* __restrict b_ptr = best.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(b_ptr, 0, group_count * sizeof(uint32_t));

    for (std::size_t i = 0; i < n; ++i) {
      uint32_t grp = g_ptr[i];
      uint32_t val = v_ptr[i];
      if (val > b_ptr[grp]) {
        b_ptr[grp] = val;
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
  }
  return hash;
}
