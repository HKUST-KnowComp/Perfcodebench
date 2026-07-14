#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const uint32_t* val_ptr = values.data();
  const uint32_t* probe_ptr = probes.data();
  const std::size_t probe_count = probes.size();
  const uint32_t base = keys.empty() ? 0 : keys[0];
  
  uint64_t hash = 0;
  const uint64_t prime = 1099511628211ULL;
  const uint64_t offset = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = offset;
    for (std::size_t i = 0; i < probe_count; ++i) {
      hash ^= val_ptr[probe_ptr[i] - base];
      hash *= prime;
    }
  }
  return hash;
}
