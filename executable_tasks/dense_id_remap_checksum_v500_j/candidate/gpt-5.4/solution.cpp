#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix_u64(uint64_t hash, uint64_t value) {
  return (hash ^ value) * 1099511628211ULL;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const std::size_t n = keys.size();
  const std::size_t m = probes.size();

  if (iters <= 0) return 0;
  if (m == 0) return 1469598103934665603ULL;

  const uint32_t* const k = n ? keys.data() : nullptr;
  const uint32_t* const v = values.data();
  const uint32_t* const p = probes.data();

  // Dense-id remap fast path: keys are expected to be dense and ordered.
  // Build a direct lookup table once, then each probe becomes a single indexed load.
  if (n != 0) {
    const uint32_t base = k[0];
    bool dense = true;
    for (std::size_t i = 1; i < n; ++i) {
      if (k[i] != base + static_cast<uint32_t>(i)) {
        dense = false;
        break;
      }
    }

    if (dense) {
      uint64_t hash = 0;
      for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;

        std::size_t i = 0;
        const std::size_t unroll = m & ~std::size_t(3);
        for (; i < unroll; i += 4) {
          hash = mix_u64(hash, v[static_cast<std::size_t>(p[i] - base)]);
          hash = mix_u64(hash, v[static_cast<std::size_t>(p[i + 1] - base)]);
          hash = mix_u64(hash, v[static_cast<std::size_t>(p[i + 2] - base)]);
          hash = mix_u64(hash, v[static_cast<std::size_t>(p[i + 3] - base)]);
        }
        for (; i < m; ++i) {
          hash = mix_u64(hash, v[static_cast<std::size_t>(p[i] - base)]);
        }
      }
      return hash;
    }
  }

  // Fallback: lower_bound equivalent specialized for raw pointers.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t probe = p[i];
      std::size_t lo = 0;
      std::size_t hi = n;
      while (lo < hi) {
        const std::size_t mid = lo + ((hi - lo) >> 1);
        if (k[mid] < probe) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      hash = mix_u64(hash, v[lo]);
    }
  }
  return hash;
}
