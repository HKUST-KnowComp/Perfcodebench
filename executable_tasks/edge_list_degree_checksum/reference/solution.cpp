#include "interface.h"

#include <vector>

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
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(degrees.begin(), degrees.end(), 0);
    for (uint32_t u : src) {
      ++degrees[static_cast<std::size_t>(u)];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t degree : degrees) {
      hash = mix(hash, degree);
    }
  }
  return hash;
}
