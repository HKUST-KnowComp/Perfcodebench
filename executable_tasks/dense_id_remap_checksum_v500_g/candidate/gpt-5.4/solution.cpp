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
  for (uint32_t p : probes) {
    if (p > max_key) max_key = p;
  }

  std::vector<uint32_t> dense(static_cast<std::size_t>(max_key) + 1);

  std::size_t j = 0;
  uint32_t current = values[0];
  for (uint32_t id = 0; id <= max_key; ++id) {
    while (j < n && keys[j] <= id) {
      current = values[j];
      ++j;
    }
    dense[id] = current;
  }

  const uint32_t* probe_data = probes.data();
  const std::size_t m = probes.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    std::size_t i = 0;
    for (; i + 4 <= m; i += 4) {
      h = mix(h, dense[probe_data[i]]);
      h = mix(h, dense[probe_data[i + 1]]);
      h = mix(h, dense[probe_data[i + 2]]);
      h = mix(h, dense[probe_data[i + 3]]);
    }
    for (; i < m; ++i) {
      h = mix(h, dense[probe_data[i]]);
    }
    hash = h;
  }
  return hash;
}
