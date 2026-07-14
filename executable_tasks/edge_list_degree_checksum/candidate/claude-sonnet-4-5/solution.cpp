#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices));
  uint64_t hash = 0;
  const std::size_t edge_count = src.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(degrees.data(), 0, static_cast<std::size_t>(vertices) * sizeof(uint32_t));
    
    for (std::size_t i = 0; i < edge_count; ++i) {
      ++degrees[static_cast<std::size_t>(src[i])];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v = 0; v < vertices; ++v) {
      hash = mix(hash, degrees[static_cast<std::size_t>(v)]);
    }
  }
  return hash;
}