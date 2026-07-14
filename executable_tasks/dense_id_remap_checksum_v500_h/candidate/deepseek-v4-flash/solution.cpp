#include "interface.h"
#include <algorithm>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const std::size_t num_probes = probes.size();
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  const uint32_t* probes_ptr = probes.data();

  // Precompute remapped values
  std::vector<uint64_t> remapped(num_probes);
  uint64_t* remapped_ptr = remapped.data();
  const std::size_t keys_size = keys.size();

  for (std::size_t i = 0; i < num_probes; ++i) {
    uint32_t probe = probes_ptr[i];
    const uint32_t* it = std::lower_bound(keys_ptr, keys_ptr + keys_size, probe);
    std::size_t idx = it - keys_ptr;
    remapped_ptr[i] = values_ptr[idx];
  }

  // Compute checksum over iters iterations
  uint64_t hash = 0;
  const uint64_t init_hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = init_hash;
    for (std::size_t i = 0; i < num_probes; ++i) {
      hash ^= remapped_ptr[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
