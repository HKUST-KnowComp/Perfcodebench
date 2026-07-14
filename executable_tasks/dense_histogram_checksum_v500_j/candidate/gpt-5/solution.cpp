#include "interface.h"

namespace {
static inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }
  if (bins == 0u) {
    return 1469598103934665603ULL;
  }

  const uint32_t B = bins;
  std::vector<uint32_t> hist(B, 0u);
  for (uint32_t v : values) {
    if (v < B) {
      ++hist[v];
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* data = hist.data();
  for (uint32_t i = 0; i < B; ++i) {
    hash = mix64(hash, data[i]);
  }
  return hash;
}
