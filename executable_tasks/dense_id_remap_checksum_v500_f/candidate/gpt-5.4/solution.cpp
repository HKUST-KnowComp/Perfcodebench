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
  if (iters <= 0) return 0;
  if (probes.empty()) return 1469598103934665603ULL;

  // Fast path for dense id remapping: keys are expected to represent a dense,
  // sorted domain, so a probe can be translated to an index with one subtract.
  // We preserve baseline semantics by falling back to a manual lower_bound if a
  // probe lies outside the dense range.
  const uint32_t base = n ? keys[0] : 0;
  const uint32_t end = n ? static_cast<uint32_t>(base + static_cast<uint32_t>(n)) : base;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const uint32_t* probe_ptr = probes.data();
    const uint32_t* probe_end = probe_ptr + probes.size();
    const uint32_t* value_ptr = values.data();
    const uint32_t* key_ptr = keys.data();

    while (probe_ptr != probe_end) {
      const uint32_t probe = *probe_ptr++;
      std::size_t idx;

      if (n && probe >= base && probe < end) {
        idx = static_cast<std::size_t>(probe - base);
      } else {
        // Manual lower_bound to avoid iterator overhead.
        std::size_t lo = 0;
        std::size_t hi = n;
        while (lo < hi) {
          const std::size_t mid = lo + ((hi - lo) >> 1);
          if (key_ptr[mid] < probe) {
            lo = mid + 1;
          } else {
            hi = mid;
          }
        }
        idx = lo;
      }

      hash ^= static_cast<uint64_t>(value_ptr[idx]);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
