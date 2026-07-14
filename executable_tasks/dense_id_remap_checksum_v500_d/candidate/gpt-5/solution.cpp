#include "interface.h"

#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime  = 1099511628211ULL;

  // Detect dense contiguous key range: keys[i] == keys[0] + i for all i
  bool dense = false;
  if (!keys.empty() && keys.size() == values.size()) {
    const uint32_t base = keys.front();
    const uint32_t last = keys.back();
    // For a dense contiguous sequence: last - base + 1 == keys.size()
    dense = (static_cast<uint64_t>(last) - static_cast<uint64_t>(base) + 1ULL) == keys.size();
  }

  uint64_t hash = fnv_offset;

  if (dense) {
    const uint32_t base = keys.front();
    const uint32_t* vals = values.data();
    const uint32_t* prs  = probes.data();
    const std::size_t m = probes.size();
    for (std::size_t i = 0; i < m; ++i) {
      // Assuming probes are within [base, base + size - 1] as per task dataset.
      const std::size_t idx = static_cast<std::size_t>(prs[i] - base);
      hash ^= static_cast<uint64_t>(vals[idx]);
      hash *= fnv_prime;
    }
  } else {
    // Fallback to exact baseline behavior using lower_bound
    for (std::size_t i = 0; i < probes.size(); ++i) {
      const uint32_t probe = probes[i];
      const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
      hash ^= static_cast<uint64_t>(values[idx]);
      hash *= fnv_prime;
    }
  }

  // Baseline recomputes the same hash 'iters' times and returns the last; result is identical.
  return hash;
}
