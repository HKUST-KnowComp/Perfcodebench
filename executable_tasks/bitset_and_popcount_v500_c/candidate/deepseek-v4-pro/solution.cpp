#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int /*iters*/) {
  uint64_t count = 0;
  std::size_t n = left.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint64_t word = left[i] & right[i];
    // Fast 64-bit popcount (no hardware requirement)
    word = word - ((word >> 1) & 0x5555555555555555ULL);
    word = (word & 0x3333333333333333ULL) + ((word >> 2) & 0x3333333333333333ULL);
    word = (word + (word >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    count += (word * 0x0101010101010101ULL) >> 56;
  }
  return count;
}