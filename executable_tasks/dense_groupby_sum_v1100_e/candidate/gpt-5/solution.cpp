#include "interface.h"

namespace {

inline uint64_t fnv1a_hash_groups(const std::vector<uint64_t>& agg, const std::vector<uint32_t>& seen, uint32_t gen, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    uint64_t v = (seen[static_cast<std::size_t>(g)] == gen) ? agg[static_cast<std::size_t>(g)] : 0ULL;
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Match baseline behavior for groups == 0
  if (groups <= 0) {
    return (iters > 0) ? 1469598103934665603ULL : 0ULL;
  }

  const std::size_t n = keys.size();

  // Dense aggregation storage
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));
  // Generation markers to avoid clearing agg each iteration
  std::vector<uint32_t> seen(static_cast<std::size_t>(groups), 0U);
  uint32_t gen = 1U;

  uint64_t out = 0ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate per key
    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = kptr[i];
      // Assume keys are bounded: 0 <= g < groups
      if (seen[g] != gen) {
        seen[g] = gen;
        agg[g] = 0ULL;
      }
      agg[g] += static_cast<uint64_t>(vptr[i]);
    }

    // Compute checksum for this iteration
    out = fnv1a_hash_groups(agg, seen, gen, groups);

    // Advance generation; handle wrap-around by resetting markers
    ++gen;
    if (gen == 0U) {
      for (int g = 0; g < groups; ++g) {
        seen[static_cast<std::size_t>(g)] = 0U;
      }
      gen = 1U;
    }
  }

  return out;
}
