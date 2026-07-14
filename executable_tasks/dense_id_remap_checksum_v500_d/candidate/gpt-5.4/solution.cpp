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

  uint32_t max_key = keys.back();
  uint32_t max_probe = 0;
  for (std::size_t i = 0, m = probes.size(); i < m; ++i) {
    if (probes[i] > max_probe) max_probe = probes[i];
  }

  const uint32_t limit = max_probe < max_key ? max_probe : max_key;

  std::vector<uint32_t> dense;
  dense.resize(static_cast<std::size_t>(limit) + 1);

  std::size_t ki = 0;
  uint32_t current = values[0];
  for (uint32_t x = 0; x <= limit; ++x) {
    while (ki + 1 < n && keys[ki] < x) {
      ++ki;
      current = values[ki];
    }
    dense[static_cast<std::size_t>(x)] = current;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint32_t* p = probes.data();
    const std::size_t m = probes.size();
    for (std::size_t i = 0; i < m; ++i) {
      hash = mix(hash, dense[p[i] <= limit ? p[i] : limit]);
    }
  }
  return hash;
}
