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
  std::vector<uint32_t> left;
  std::vector<uint32_t> right;
  std::vector<uint32_t> out;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    left.clear();
    right.clear();
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        left.push_back(values[i]);
      } else {
        right.push_back(values[i]);
      }
    }
    out = left;
    out.insert(out.end(), right.begin(), right.end());
    hash = 1469598103934665603ULL;
    for (uint32_t v : out) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
