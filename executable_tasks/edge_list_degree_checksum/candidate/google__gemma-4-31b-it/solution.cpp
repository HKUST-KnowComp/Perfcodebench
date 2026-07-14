#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  // Pre-allocate degree buffer to avoid repeated allocations inside the loop
  std::vector<uint32_t> degrees(vertices);
  uint64_t hash = 0;
  
  const uint32_t* src_ptr = src.data();
  const size_t src_size = src.size();
  uint32_t* deg_ptr = degrees.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset degrees to zero
    std::memset(deg_ptr, 0, vertices * sizeof(uint32_t));

    // Calculate out-degrees
    for (size_t i = 0; i < src_size; ++i) {
      deg_ptr[src_ptr[i]]++;
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v = 0; v < vertices; ++v) {
      hash = mix(hash, deg_ptr[v]);
    }
  }
  
  return hash;
}