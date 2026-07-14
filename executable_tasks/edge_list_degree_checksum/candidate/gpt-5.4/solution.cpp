#include "interface.h"

#include <cstdint>
#include <vector>

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

  const std::size_t vcount = static_cast<std::size_t>(vertices);
  std::vector<uint32_t> degrees(vcount, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    if (iter != 0) {
      for (std::size_t i = 0; i < vcount; ++i) {
        degrees[i] = 0;
      }
    }

    const std::size_t m = src.size();
    for (std::size_t i = 0; i < m; ++i) {
      ++degrees[static_cast<std::size_t>(src[i])];
    }

    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < vcount; ++i) {
      h = mix(h, degrees[i]);
    }
    hash = h;
  }

  return hash;
}
