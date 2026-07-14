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
  const uint32_t* __restrict val_ptr = values.data();
  const uint32_t* __restrict probe_ptr = probes.data();
  const std::size_t probe_count = probes.size();
  const uint32_t key_offset = keys.empty() ? 0 : keys[0];
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = probe_count - (probe_count % 4);
    
    // 4-way unrolled loop
    for (; i < unroll_limit; i += 4) {
      uint32_t p0 = probe_ptr[i] - key_offset;
      uint32_t p1 = probe_ptr[i + 1] - key_offset;
      uint32_t p2 = probe_ptr[i + 2] - key_offset;
      uint32_t p3 = probe_ptr[i + 3] - key_offset;
      
      hash = mix(hash, val_ptr[p0]);
      hash = mix(hash, val_ptr[p1]);
      hash = mix(hash, val_ptr[p2]);
      hash = mix(hash, val_ptr[p3]);
    }
    
    // Handle remainder
    for (; i < probe_count; ++i) {
      uint32_t idx = probe_ptr[i] - key_offset;
      hash = mix(hash, val_ptr[idx]);
    }
  }
  
  return hash;
}