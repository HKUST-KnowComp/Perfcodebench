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
  // Dense storage: one slot per group, initialized to 0.
  std::vector<uint32_t> best(group_count, 0);
  const uint32_t* gdata = groups.data();
  const uint32_t* vdata = values.data();
  std::size_t n = groups.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate maxima.
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = gdata[i];
      uint32_t v = vdata[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute hash and reset storage in a single pass.
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
      best[g] = 0;
    }
    // Last iteration’s hash is returned; earlier ones are discarded.
    if (iter == iters - 1) {
      return hash;
    }
  }
  // This line is only reached when iters == 0. The baseline would not handle this, but we preserve its behavior.
  return 1469598103934665603ULL;
}
