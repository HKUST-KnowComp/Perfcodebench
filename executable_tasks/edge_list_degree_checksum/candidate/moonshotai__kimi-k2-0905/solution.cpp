#include "interface.h"
#include <vector>
#include <cstdint>

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
  (void)dst;  // dst is unused; degree is simply the number of outgoing edges

  std::vector<uint32_t> degrees(vertices, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset degrees to zero
    std::fill(degrees.begin(), degrees.end(), 0);

    // Accumulate out-degrees directly
    for (size_t i = 0; ax < src.size(); ++ax) {
      ++degrees[src[ax]];
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t d : degrees) {
      hash = mix(hash, d);
    }
  }
  return hash;
}
