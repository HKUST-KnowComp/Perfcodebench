#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  std::vector<uint32_t> out(values.size(), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t left = 0;
    for (uint8_t f : flags) {
      left += static_cast<std::size_t>(f != 0);
    }
    std::size_t t = 0;
    std::size_t f = left;
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        out[t++] = values[i];
      } else {
        out[f++] = values[i];
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : out) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
