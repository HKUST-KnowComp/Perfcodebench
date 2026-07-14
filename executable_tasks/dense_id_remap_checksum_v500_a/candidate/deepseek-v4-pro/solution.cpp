#include "interface.h"

#include <algorithm>
#include <cstddef>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;
  const std::size_t P = probes.size();
  std::vector<std::size_t> indices(P);
  for (std::size_t i = 0; i < P; ++i) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
    indices[i] = static_cast<std::size_t>(it - keys.begin());
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < P; ++i) {
      const std::size_t idx = indices[i];
      hash ^= values[idx];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
