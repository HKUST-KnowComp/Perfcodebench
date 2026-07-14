#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  (void)keys;
  uint64_t hash = 0;
  const uint32_t* val_ptr = values.data();
  const uint32_t* probe_ptr = probes.data();
  const size_t n = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      uint64_t v0 = val_ptr[probe_ptr[i]];
      hash ^= v0;
      hash *= 1099511628211ULL;
      
      uint64_t v1 = val_ptr[probe_ptr[i+1]];
      hash ^= v1;
      hash *= 1099511628211ULL;
      
      uint64_t v2 = val_ptr[probe_ptr[i+2]];
      hash ^= v2;
      hash *= 1099511628211ULL;
      
      uint64_t v3 = val_ptr[probe_ptr[i+3]];
      hash ^= v3;
      hash *= 1099511628211ULL;
    }
    for (; i < n; ++i) {
      uint64_t v = val_ptr[probe_ptr[i]];
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}