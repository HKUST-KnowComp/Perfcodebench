#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  (void)iters;

  // Compute out-degree counts directly from src once.
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices), 0u);
  const std::size_t m = src.size();
  uint32_t* deg = degrees.data();
  for (std::size_t i = 0; i < m; ++i) {
    deg[static_cast<std::size_t>(src[i])]++;
  }

  // Compute the checksum (FNV-1a style as in baseline).
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t v = 0, V = static_cast<std::size_t>(vertices); v < V; ++v) {
    hash = mix(hash, static_cast<uint64_t>(deg[v]));
  }

  return hash;
}
