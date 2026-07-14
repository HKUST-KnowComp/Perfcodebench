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
  // Dense storage for maxima per group and a version tag to avoid clearing each iteration.
  std::vector<uint32_t> maxima(group_count);
  std::vector<uint32_t> tags(group_count, 0);

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const std::size_t N = groups.size();

  uint64_t hash = 0;
  uint32_t currentVersion = 0;  // will increment before use

  for (int iter = 0; iter < iters; ++iter) {
    // Advance version; on wrap, reset tags to 0 and set version to 1.
    currentVersion += 1u;
    if (currentVersion == 0u) {
      // Rare overflow path: clear tags and restart versioning.
      for (uint32_t i = 0; i < group_count; ++i) tags[i] = 0u;
      currentVersion = 1u;
    }

    // Accumulate per-group maxima.
    for (std::size_t i = 0; i < N; ++i) {
      const uint32_t g = gptr[i];
      if (g >= group_count) continue;  // ignore out-of-range groups (consistent with baseline hash loop)
      const uint32_t val = vptr[i];
      uint32_t& tag = tags[g];
      uint32_t& maxv = maxima[g];
      if (tag == currentVersion) {
        if (val > maxv) maxv = val;
      } else {
        tag = currentVersion;
        maxv = val;
      }
    }

    // Compute hash over all groups in order, using 0 for groups not seen this iteration.
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      const uint32_t val = (tags[g] == currentVersion) ? maxima[g] : 0u;
      h = mix(h, val);
    }
    hash = h;  // match baseline: return the hash from the last iteration
  }

  return hash;
}
