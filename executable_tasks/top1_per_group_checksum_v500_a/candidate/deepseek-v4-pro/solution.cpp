#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint32_t> stamp(group_count, 0);
  uint32_t cur_stamp = 0;
  uint64_t hash = 0;

  const uint32_t* groups_ptr = groups.data();
  const uint32_t* values_ptr = values.data();
  const std::size_t n = groups.size();

  for (int iter = 0; iter < iters; ++iter) {
    ++cur_stamp;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = groups_ptr[i];
      const uint32_t v = values_ptr[i];
      if (stamp[g] != cur_stamp) {
        stamp[g] = cur_stamp;
        best[g] = 0;
      }
      if (v > best[g]) {
        best[g] = v;
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      const uint32_t val = (stamp[g] == cur_stamp) ? best[g] : 0;
      hash = mix(hash, val);
    }
  }
  return hash;
}