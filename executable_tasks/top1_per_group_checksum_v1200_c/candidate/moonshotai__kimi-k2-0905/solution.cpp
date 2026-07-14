#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count, 0u);
  uint64_t hash = 0;
  const size_t n = groups.size();

  for (int iter = 0; iter < iters; ++iter) {
    // reset all maxima
    std::fill(best.begin(), best.end(), 0u);

    // update maxima
    for (size_t i = 0; i < n; ++i) {
      const uint32_t g = groups[i];
      const uint32_t v = values[i];
      if (v > best[g]) best[g] = v;
    }

    // compute checksum
    uint64_t h = 1469598103934665603ULL;
    const uint32_t* ptr = best.data();
    const uint32_t* const end = ptr + group_count;
    while (ptr + 4 <= end) {
      h = mix(h, ptr[0]);
      h = mix(h, ptr[1]);
      h = mix(h, ptr[2]);
      h = mix(h, ptr[3]);
      ptr += 4;
    }
    while (ptr < end) {
      h = mix(h, *ptr++);
    }
    hash = h;
  }
  return hash;
}