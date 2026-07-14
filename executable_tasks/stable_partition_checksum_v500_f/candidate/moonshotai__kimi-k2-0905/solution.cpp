#include "interface.h"
#include <vector>
#include <cstdint>

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
  const std::size_t n = values.size();
  std::vector<uint32_t> out(n);
  std::size_t left_pos = 0;
  std::size_t right_pos = 0;

  // count left elements once
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) ++right_pos;   // right_pos == left_count after loop
  }
  left_pos = right_pos;        // left_pos is first slot for right block
  right_pos = 0;

  // build the stable order once
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      out[right_pos++] = values[i];
    } else {
      out[left_pos++] = values[i];
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = hash;
    for (uint32_t v : out) {
      h = mix(h, v);
    }
    hash = h;
  }
  return hash;
}