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
  std::vector<std::vector<uint32_t>> adjacency;
  std::vector<uint32_t> degrees;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    adjacency.assign(static_cast<std::size_t>(vertices), {});
    for (std::size_t i = 0; i < src.size(); ++i) {
      adjacency[static_cast<std::size_t>(src[i])].push_back(dst[i]);
    }
    degrees.resize(static_cast<std::size_t>(vertices));
    for (uint32_t v = 0; v < vertices; ++v) {
      degrees[static_cast<std::size_t>(v)] =
          static_cast<uint32_t>(adjacency[static_cast<std::size_t>(v)].size());
    }
    hash = 1469598103934665603ULL;
    for (uint32_t degree : degrees) {
      hash = mix(hash, degree);
    }
  }
  return hash;
}
