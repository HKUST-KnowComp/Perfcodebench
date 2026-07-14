#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const std::size_t n = keys.size();
  if (n == 0 || probes.empty() || iters <= 0) {
    return 0;
  }

  uint32_t min_key = keys.front();
  uint32_t max_key = keys.back();

  // Fast path for dense, contiguous ids: direct indexing table.
  // This matches the task's intended workload and avoids per-probe binary search.
  const uint64_t span64 = static_cast<uint64_t>(max_key) - static_cast<uint64_t>(min_key) + 1ULL;
  if (span64 == n) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;

      const uint32_t* probe_ptr = probes.data();
      const uint32_t* probe_end = probe_ptr + probes.size();
      const uint32_t* value_base = values.data();

      for (; probe_ptr != probe_end; ++probe_ptr) {
        const uint32_t idx = *probe_ptr - min_key;
        hash = mix(hash, value_base[idx]);
      }
    }
    return hash;
  }

  // Fallback: branch-light lower_bound specialized for random-access arrays.
  uint64_t hash = 0;
  const uint32_t* key_base = keys.data();
  const uint32_t* value_base = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const uint32_t* probe_ptr = probes.data();
    const uint32_t* probe_end = probe_ptr + probes.size();
    for (; probe_ptr != probe_end; ++probe_ptr) {
      const uint32_t probe = *probe_ptr;
      std::size_t first = 0;
      std::size_t count = n;
      while (count > 0) {
        const std::size_t step = count >> 1;
        const std::size_t mid = first + step;
        if (key_base[mid] < probe) {
          first = mid + 1;
          count -= step + 1;
        } else {
          count = step;
        }
      }
      hash = mix(hash, value_base[first]);
    }
  }

  return hash;
}
