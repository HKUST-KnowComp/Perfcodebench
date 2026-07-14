#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Handle trivial cases quickly
  if (iters <= 0) return 0;
  if (packed.empty()) return 0;
  // If threshold >= 15, no 4-bit value (0..15) is greater than threshold
  if (threshold >= 15u) return 0;

  // Precompute per-byte contribution: sum of its two nibbles if > threshold
  uint8_t lut[256];
  const unsigned thr = static_cast<unsigned>(threshold);
  for (unsigned b = 0; b < 256; ++b) {
    unsigned lo = b & 0xFu;
    unsigned hi = b >> 4;
    unsigned v = (lo > thr ? lo : 0u) + (hi > thr ? hi : 0u);
    lut[b] = static_cast<uint8_t>(v); // v <= 30
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Use multiple accumulators and unroll to improve ILP and reduce loop overhead
    uint64_t a0 = 0, a1 = 0, a2 = 0, a3 = 0;
    std::size_t i = 0;
    const std::size_t n16 = n & ~static_cast<std::size_t>(15);

    for (; i < n16; i += 16) {
      const uint8_t* p = data + i;
      a0 += lut[p[0]];  a1 += lut[p[1]];  a2 += lut[p[2]];  a3 += lut[p[3]];
      a0 += lut[p[4]];  a1 += lut[p[5]];  a2 += lut[p[6]];  a3 += lut[p[7]];
      a0 += lut[p[8]];  a1 += lut[p[9]];  a2 += lut[p[10]]; a3 += lut[p[11]];
      a0 += lut[p[12]]; a1 += lut[p[13]]; a2 += lut[p[14]]; a3 += lut[p[15]];
    }

    uint64_t acc = a0 + a1 + a2 + a3;
    for (; i < n; ++i) {
      acc += lut[data[i]];
    }

    sum = acc; // per baseline semantics: return the sum from the last iteration
  }

  return sum;
}
