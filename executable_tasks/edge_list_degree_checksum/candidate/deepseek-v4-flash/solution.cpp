#include "interface.h"
#include <cstdint>
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
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    degrees.assign(static_cast<std::size_t>(vertices), 0);
    for (std::size_t i = 0; i < src.size(); ++i) {
      ++degrees[static_cast<std::size_t>(src[i])];
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < static_cast<std::size_t>(vertices); ++i) {
      hash = mix(hash, degrees[i]);
    }
  }
  return hash;
}
