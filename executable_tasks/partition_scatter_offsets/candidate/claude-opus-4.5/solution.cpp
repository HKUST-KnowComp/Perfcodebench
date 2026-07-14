#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  const std::size_t n = keys.size();
  const std::size_t g = static_cast<std::size_t>(groups);
  
  std::vector<uint32_t> counts(g + 1, 0);
  std::vector<uint32_t> offsets(g + 1);
  std::vector<uint32_t> write_pos(g);
  std::vector<uint32_t> rows(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    for (std::size_t i = 0; i <= g; ++i) {
      counts[i] = 0;
    }
    
    // Count elements per bucket
    for (std::size_t i = 0; i < n; ++i) {
      counts[keys[i]]++;
    }
    
    // Compute prefix sums (offsets)
    offsets[0] = 0;
    for (std::size_t i = 0; i < g; ++i) {
      offsets[i + 1] = offsets[i] + counts[i];
    }
    
    // Copy offsets to write positions
    for (std::size_t i = 0; i < g; ++i) {
      write_pos[i] = offsets[i];
    }
    
    // Scatter row indices into their buckets
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      rows[write_pos[k]++] = static_cast<uint32_t>(i);
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i <= g; ++i) {
      hash = mix(hash, offsets[i]);
    }
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, rows[i]);
    }
  }
  
  return hash;
}