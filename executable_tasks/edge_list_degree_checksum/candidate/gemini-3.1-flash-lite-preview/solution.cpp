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

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  
  // Pre-allocate degree buffer to avoid reallocations
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices));
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset degrees efficiently
    std::fill(degrees.begin(), degrees.end(), 0);

    // Count out-degrees directly
    for (uint32_t s : src) {
      if (s < vertices) {
        degrees[s]++;
      }
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t degree : degrees) {
      hash = mix(hash, degree);
    }
  }
  return hash;
}