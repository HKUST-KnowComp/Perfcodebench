#include "interface.h"

#include <cstdint>

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
  
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 0;
  }

  // For dense ids, keys[i] = keys[0] + i, so index = probe - keys[0]
  const uint32_t base = keys[0];
  const uint32_t* __restrict__ vals = values.data();
  const uint32_t* __restrict__ prb = probes.data();
  const std::size_t n = probes.size();

  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    
    // Unroll by 8 for better ILP
    const std::size_t n8 = n & ~static_cast<std::size_t>(7);
    for (; i < n8; i += 8) {
      uint32_t p0 = prb[i];
      uint32_t p1 = prb[i + 1];
      uint32_t p2 = prb[i + 2];
      uint32_t p3 = prb[i + 3];
      uint32_t p4 = prb[i + 4];
      uint32_t p5 = prb[i + 5];
      uint32_t p6 = prb[i + 6];
      uint32_t p7 = prb[i + 7];
      
      uint32_t v0 = vals[p0 - base];
      uint32_t v1 = vals[p1 - base];
      uint32_t v2 = vals[p2 - base];
      uint32_t v3 = vals[p3 - base];
      uint32_t v4 = vals[p4 - base];
      uint32_t v5 = vals[p5 - base];
      uint32_t v6 = vals[p6 - base];
      uint32_t v7 = vals[p7 - base];
      
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
      hash = mix(hash, v4);
      hash = mix(hash, v5);
      hash = mix(hash, v6);
      hash = mix(hash, v7);
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t idx = prb[i] - base;
      hash = mix(hash, vals[idx]);
    }
  }
  
  return hash;
}