#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const size_t n = values.size();
  if (n == 0) return 0;

  uint64_t final_hash = 0;
  
  // Pre-calculate the count of elements that go to the left to avoid dynamic resizing
  // However, since we only need the hash, we can just iterate twice over the input
  // to simulate the stable partition without actually allocating a full output vector
  // unless the hash depends on the final sequence. 
  // The original code creates 'out' and then hashes it. 
  // We can achieve the same result by iterating through flags twice.

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;

    // First pass: elements where flags[i] == 1
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }

    // Second pass: elements where flags[i] == 0
    for (size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    final_hash = hash;
  }

  return final_hash;
}