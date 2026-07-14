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
  if (iters <= 0) return 0ULL;

  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint32_t> generation(group_count, 0);
  uint32_t cur_gen = 0;

  for (int iter = 0; iter < iters; ++iter) {
    ++cur_gen;

    const size_t n = groups.size();
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = groups[i];
      uint32_t v = values[i];
      if (generation[g] != cur_gen) {
        best[g] = v;
        generation[g] = cur_gen;
      } else if (v > best[g]) {
        best[g] = v;
      }
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      uint64_t val = (generation[g] == cur_gen) ? best[g] : 0ULL;
      hash = mix(hash, val);
    }
  }
  return hash;
}
