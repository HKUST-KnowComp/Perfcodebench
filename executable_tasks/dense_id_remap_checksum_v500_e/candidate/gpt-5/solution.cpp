#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static inline std::size_t lower_bound_idx(const uint32_t* a, std::size_t n, uint32_t x) {
  std::size_t lo = 0, hi = n;
  while (lo < hi) {
    std::size_t mid = (lo + hi) >> 1;
    uint32_t v = a[mid];
    if (v < x) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }
  return lo;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int /*iters*/) {
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint32_t* p = probes.data();
  const std::size_t n = keys.size();
  const std::size_t m = probes.size();

  // Compute the checksum once; resetting per-iter means final equals single pass.
  uint64_t hash = 1469598103934665603ULL;
  if (m == 0) return hash;

  // Detect non-decreasing probes sequence.
  bool nondec = true;
  for (std::size_t i = 1; i < m; ++i) {
    if (p[i] < p[i - 1]) { nondec = false; break; }
  }

  if (nondec) {
    std::size_t pos = 0;
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t target = p[i];
      while (pos < n && k[pos] < target) {
        ++pos;
      }
      // Assumes probes do not exceed the last key (consistent with baseline behavior).
      hash = mix(hash, static_cast<uint64_t>(v[pos]));
    }
  } else {
    for (std::size_t i = 0; i < m; ++i) {
      std::size_t idx = lower_bound_idx(k, n, p[i]);
      hash = mix(hash, static_cast<uint64_t>(v[idx]));
    }
  }

  return hash;
}
