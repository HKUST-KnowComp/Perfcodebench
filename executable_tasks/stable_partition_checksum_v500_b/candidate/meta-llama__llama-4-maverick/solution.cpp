#include "interface.h"

#include <vector>
#include <cstring> // for std::size_t

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
    std::size_t left = 0;
    std::size_t right = values.size();
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        out[left++] = values[i];
      } else {
        out[--right] = values[i];
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : out) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
