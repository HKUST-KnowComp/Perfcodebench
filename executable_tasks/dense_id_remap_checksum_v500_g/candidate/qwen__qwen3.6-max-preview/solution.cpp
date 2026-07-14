#include "interface.h"

#include <cstdint>
#include <cstddef>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const uint64_t offset_basis = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  const uint32_t* val_ptr = values.data();
  const uint32_t* probe_ptr = probes.data();
  const std::size_t n_probes = probes.size();
  const uint32_t base = keys.empty() ? 0 : keys[0];

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = offset_basis;
    for (std::size_t i = 0; i < n_probes; ++i) {
      uint32_t idx = probe_ptr[i] - base;
      hash ^= val_ptr[idx];
      hash *= prime;
    }
  }
  return hash;
}