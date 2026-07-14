#include "interface.h"

#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  std::vector<uint8_t> bits;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    bits.resize(words.size() * 64u);
    for (std::size_t i = 0; i < words.size(); ++i) {
      uint64_t word = words[i];
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
