#include "interface.h"

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty() || probes.empty() || iters <= 0) return 0;

  const uint32_t offset = keys[0];
  const uint32_t* vals = values.data();
  const uint32_t* prbs = probes.data();
  const size_t n = probes.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      h ^= vals[prbs[i] - offset];
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}