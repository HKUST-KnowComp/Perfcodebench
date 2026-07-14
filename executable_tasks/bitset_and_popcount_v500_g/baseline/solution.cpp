#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  std::vector<uint8_t> bits;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    bits.resize(left.size() * 64u);
    for (std::size_t i = 0; i < left.size(); ++i) {
      const uint64_t word = left[i] & right[i];
      for (int b = 0; b < 64; ++b) {
        bits[i * 64u + static_cast<std::size_t>(b)] = static_cast<uint8_t>((word >> b) & 1ULL);
      }
    }
    count = 0;
    for (uint8_t bit : bits) {
      count += static_cast<uint64_t>(bit);
    }
  }
  return count;
}
