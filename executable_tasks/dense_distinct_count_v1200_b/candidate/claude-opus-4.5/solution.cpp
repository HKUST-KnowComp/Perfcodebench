#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitset(num_words);
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));

    for (uint32_t key : keys) {
      bitset[key >> 6] |= (1ULL << (key & 63));
    }

    uint64_t c = 0;
    for (size_t i = 0; i < num_words; ++i) {
      c += __builtin_popcountll(bitset[i]);
    }
    count = c;
  }

  return count;
}
