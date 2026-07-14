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
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = groups.size();

  // Arrays to hold per-group max and the iteration they were last updated.
  std::vector<uint32_t> best(group_count);
  std::vector<uint32_t> seen_version(group_count, 0u);

  uint64_t hash = 0ULL;

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  for (int iter = 1; iter <= iters; ++iter) {
    const uint32_t cur_ver = static_cast<uint32_t>(iter);

    // Accumulate per-group maxima for this iteration using versioning to avoid clearing.
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      if (seen_version[g] == cur_ver) {
        if (v > best[g]) best[g] = v;
      } else {
        seen_version[g] = cur_ver;
        best[g] = v;
      }
    }

    // Compute hash of per-group maxima (or 0 if group not seen this iter).
    hash = 1469598103934665603ULL; // FNV-1a offset basis
    for (uint32_t g = 0; g < group_count; ++g) {
      const uint32_t val = (seen_version[g] == cur_ver) ? best[g] : 0u;
      hash = mix(hash, val);
    }
  }

  return hash;
}
