#include "interface.h"

#include <cstring>
#include <vector>

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices));
  uint64_t hash = 0;
  const uint32_t* src_data = src.data();
  const std::size_t edge_count = src.size();
  uint32_t* deg_data = degrees.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(deg_data, 0, static_cast<std::size_t>(vertices) * sizeof(uint32_t));
    
    for (std::size_t i = 0; i < edge_count; ++i) {
      ++deg_data[src_data[i]];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v = 0; v < vertices; ++v) {
      hash ^= deg_data[v];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
