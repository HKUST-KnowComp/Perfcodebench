#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  std::vector<uint32_t> degrees(vertices, 0);
  uint64_t hash = 0;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(degrees.begin(), degrees.end(), 0);
    for (uint32_t s : src) {
      degrees[s]++;
    }
    hash = FNV_OFFSET;
    for (uint32_t d : degrees) {
      hash ^= d;
      hash *= FNV_PRIME;
    }
  }
  return hash;
}
