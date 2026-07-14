#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t hash = 0;
  const uint32_t* __restrict__ val_ptr = values.data();
  const uint32_t* __restrict__ probe_ptr = probes.data();
  const std::size_t n_probes = probes.size();
  const uint32_t base = keys.empty() ? 0 : keys[0];

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n_probes; ++i) {
      if (i + 16 < n_probes) {
        __builtin_prefetch(&val_ptr[probe_ptr[i + 16] - base], 0, 0);
      }
      hash ^= val_ptr[probe_ptr[i] - base];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}