#include "interface.h"

#include <algorithm>
#include <cstring>

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
  
  if (keys.empty() || probes.empty()) {
    return 1469598103934665603ULL;
  }
  
  // Build direct lookup table
  const uint32_t max_key = keys.back();
  std::vector<uint32_t> remap(max_key + 1);
  
  for (size_t i = 0; i < keys.size(); ++i) {
    remap[keys[i]] = values[i];
  }
  
  uint64_t hash = 0;
  const size_t probe_count = probes.size();
  const size_t main_loop = probe_count / 4 * 4;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Unrolled loop for main body
    size_t i = 0;
    for (; i < main_loop; i += 4) {
      hash = mix(hash, remap[probes[i]]);
      hash = mix(hash, remap[probes[i + 1]]);
      hash = mix(hash, remap[probes[i + 2]]);
      hash = mix(hash, remap[probes[i + 3]]);
    }
    
    // Handle remainder
    for (; i < probe_count; ++i) {
      hash = mix(hash, remap[probes[i]]);
    }
  }
  
  return hash;
}