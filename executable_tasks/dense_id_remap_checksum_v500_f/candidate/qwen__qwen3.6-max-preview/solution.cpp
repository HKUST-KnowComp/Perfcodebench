#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t hash = 0;
  uint32_t offset = keys.empty() ? 0 : keys[0];
  const uint32_t* val_data = values.data();
  const uint32_t* probe_data = probes.data();
  size_t n_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n_probes; ++i) {
      hash ^= val_data[probe_data[i] - offset];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}