#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t final_count = 0;
  const std::size_t word_count = left.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < word_count; ++i) {
      const uint64_t and_word = left[i] & right[i];
      current_sum += __builtin_popcountll(and_word);
    }
    final_count = current_sum;
  }
  return final_count;
}