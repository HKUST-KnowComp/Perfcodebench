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
  const std::size_t m = probes.size();

  if (iters <= 0) return 0;
  if (m == 0) return 1469598103934665603ULL;

  const uint32_t* const keys_data = n ? keys.data() : nullptr;
  const uint32_t* const values_data = values.data();
  const uint32_t* const probes_data = probes.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t probe = probes_data[i];

      std::size_t lo = 0;
      std::size_t hi = n;
      while (lo < hi) {
        const std::size_t mid = lo + ((hi - lo) >> 1);
        if (keys_data[mid] < probe) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }

      hash = mix(hash, values_data[lo]);
    }
  }

  return hash;
}
