#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys,
                        uint32_t key_space,
                        int iters) {
  // key_space ≤ 500 according to the task description, so a 512-bit bitmap
  // (64 bytes) fits comfortably in L1 and is faster than std::unordered_set
  // for such tiny sets that are cleared each iteration.
  alignas(64) uint64_t bits[8] = {0};
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t k : keys) {
      uint32_t idx = k >> 6;          // word index
      uint32_t mask = 1ULL << (k & 63); // bit mask
      bits[idx] |= mask;
    }
    // Popcount the words we have set; key_space≤500 so we need only 8 words.
    uint64_t pop = __builtin_popcountll(bits[0])
                   + __builtin_popcountll(bits[1])
                   + __builtin_popcountll(bits[2])
                   + __builtin_popcountll(bits[3])
                   + __builtin_popcountll(bits[4])
                   + __builtin_popcountll(bits[5])
                   + __builtin_popcountll(bits[6])
                   + __builtin_popcountll(bits[7]);
    count = pop;

    // Reset only the words we touched during this scan
    if (bits[0]) bits[0] = 0;
    if (bits[1]) bits[1] = 0;
    if (bits[2]) bits[2] = 0;
    if (bits[3]) bits[3] = 0;
    if (bits[4]) bits[4] = 0;
    if (bits[5]) bits[5] = 0;
    if (bits[6]) bits[6] = 0;
    if (bits[7]) bits[7] = 0;
  }
  return count;
}