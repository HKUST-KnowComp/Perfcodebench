#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0 || keys.empty()) return 0;

  // Bounded key space allows a dense marking array, which is much faster than
  // hashing for repeated exact distinct counts.
  std::vector<uint8_t> seen(static_cast<size_t>(key_space), uint8_t{0});

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t distinct = 0;
    for (uint32_t key : keys) {
      // Per task contract keys are bounded by key_space.
      uint8_t &slot = seen[static_cast<size_t>(key)];
      if (!slot) {
        slot = 1;
        ++distinct;
      }
    }
    count = distinct;

    // Clear the dense marks for the next iteration. Scanning the full bounded
    // key space is acceptable for this cold-key-scan task shape.
    for (uint32_t i = 0; i < key_space; ++i) {
      seen[static_cast<size_t>(i)] = 0;
    }
  }
  return count;
}
