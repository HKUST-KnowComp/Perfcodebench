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
  std::vector<uint32_t> out(values.size());
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t k = 0;
    for (std::size_t i = 0; i < flags.size(); ++i) {
      if (flags[i]) {
        k++;
      }
    }
    size_t left_idx = 0;
    size_t right_idx = k;
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        out[left_idx++] = values[i];
      } else {
        out[right_idx++] = values[i];
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : out) {
      hash = mix(hash, v);
    }
  }
  return hash;
}