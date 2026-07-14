#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Buffers reused across iterations
  std::vector<uint32_t> best(group_count);
  std::vector<uint32_t> seen(group_count, 0u);

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const std::size_t n = groups.size();

  uint64_t hash = 0;
  // Use 1-based stamps so 0 means "unseen in current iter"
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t stamp = static_cast<uint32_t>(iter + 1);

    // Compute per-group maxima for this iteration
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      if (g >= group_count) {
        continue; // Ignore groups outside [0, group_count)
      }
      const uint32_t val = vptr[i];
      if (seen[g] != stamp) {
        seen[g] = stamp;
        best[g] = val;
      } else if (val > best[g]) {
        best[g] = val;
      }
    }

    // Compute FNV-1a hash over group maxima in order [0..group_count)
    uint64_t h = FNV_OFFSET;
    for (uint32_t g = 0; g < group_count; ++g) {
      const uint32_t v = (seen[g] == stamp) ? best[g] : 0u;
      h ^= static_cast<uint64_t>(v);
      h *= FNV_PRIME;
    }
    hash = h; // Per baseline, return last iteration's hash
  }

  return hash;
}