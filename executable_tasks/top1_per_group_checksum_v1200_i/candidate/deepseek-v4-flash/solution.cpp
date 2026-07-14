#include "interface.h"
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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Single pass over all records
    const auto* g_ptr = groups.data();
    const auto* v_ptr = values.data();
    for (size_t i = 0; i < groups.size(); ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > best[g]) best[g] = v;
    }
    // Compute checksum and reset best for next iteration
    hash = 1469598103934665603ULL;
    uint32_t* b_ptr = best.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
      b_ptr[g] = 0;
    }
  }
  return hash;
}