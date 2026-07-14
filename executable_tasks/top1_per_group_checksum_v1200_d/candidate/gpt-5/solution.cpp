#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const std::size_t n = groups.size();

  // Dense arrays for per-group maxima and per-iteration stamps.
  std::vector<uint32_t> best(group_count);
  std::vector<uint32_t> stamps(group_count, 0);
  uint32_t stamp_id = 1;  // Current iteration mark; 0 means "unset".

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate maxima per group for this iteration using stamp markers.
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t val = vptr[i];
      if (stamps[g] != stamp_id) {
        stamps[g] = stamp_id;
        best[g] = val;
      } else if (val > best[g]) {
        best[g] = val;
      }
    }

    // Compute hash of all groups in order, using 0 for groups not seen this iter.
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      const uint64_t v = (stamps[g] == stamp_id) ? static_cast<uint64_t>(best[g]) : 0ULL;
      h = mix(h, v);
    }
    hash = h;

    // Advance stamp id; on wraparound, reset stamps to 0 to preserve correctness.
    ++stamp_id;
    if (stamp_id == 0) {
      // Extremely unlikely, but guarantees correctness on 32-bit wrap.
      for (uint32_t g = 0; g < group_count; ++g) stamps[g] = 0;
      stamp_id = 1;
    }
  }

  return hash;
}
