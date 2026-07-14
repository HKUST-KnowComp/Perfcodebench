#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  uint64_t hash = 0;
  const uint32_t* values_ptr = values.data();
  const uint32_t* probes_ptr = probes.data();
  const std::size_t probes_size = probes.size();
  const std::size_t values_size = values.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < probes_size; ++i) {
      uint32_t probe = probes_ptr[i];
      if (probe < values_size) {
        hash = mix(hash, values_ptr[probe]);
      }
    }
  }
  return hash;
}