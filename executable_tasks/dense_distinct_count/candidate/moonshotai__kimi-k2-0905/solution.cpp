#include "interface.h"

#include <cstdint>
#include <vector>

// Works for any key_space up to 2^32-1.  If key_space is not a multiple of 64
// we simply allocate one extra word and ignore the surplus bits.
uint64_t distinct_count(const std::vector<uint32_t>& keys,
                        uint32_t key_space,
                        int iters) {
  const uint32_t kBits = key_space;
  const uint32_t kWords = (kBits + 63) >> 6;
  static thread_local std::vector<uint64_t> bits;
  if (bits.size() < kWords) {
    bits.resize(kWords);
  }

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all words to zero with 64-bit stores
    uint64_t* p = bits.data();
    for (uint32_t i = 0; i < kWords; ++i) {
      p[i] = 0;
    }

    for (uint32_t key : keys) {
      const uint32_t idx = key >> 6;
      const uint64_t mask = uint64_t(1) << (key & 63);
      p[idx] |= mask;
    }

    // Population count
    uint64_t pop = 0;
    for (uint32_t i = 0; i < kWords; ++i) {
      pop += __builtin_popcountll(p[i]);
    }
    count = pop;
  }
  return count;
}