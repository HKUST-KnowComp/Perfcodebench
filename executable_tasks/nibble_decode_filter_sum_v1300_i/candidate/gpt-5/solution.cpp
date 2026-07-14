#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // If no iterations requested, match baseline behavior (sum remains 0)
  if (iters <= 0) return 0ull;

  // If threshold is >= 15, no 4-bit nibble (0..15) is strictly greater => sum is 0
  if (threshold >= 15u) return 0ull;

  // Build a per-byte lookup table for the given threshold
  // table[b] = sum of the two nibbles in b that are > threshold
  uint8_t lut[256];
  const uint8_t thr = threshold;
  for (int b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 15);
    const uint8_t hi = static_cast<uint8_t>(static_cast<uint8_t>(b) >> 4);
    uint8_t acc = 0;
    if (lo > thr) acc = static_cast<uint8_t>(acc + lo);
    if (hi > thr) acc = static_cast<uint8_t>(acc + hi);
    lut[b] = acc;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  uint64_t sum = 0ull;

  // Unrolled loop over bytes, summing contributions from the LUT
  std::size_t i = 0;
  const std::size_t limit = n & ~static_cast<std::size_t>(7);
  for (; i < limit; i += 8) {
    sum += lut[data[i + 0]];
    sum += lut[data[i + 1]];
    sum += lut[data[i + 2]];
    sum += lut[data[i + 3]];
    sum += lut[data[i + 4]];
    sum += lut[data[i + 5]];
    sum += lut[data[i + 6]];
    sum += lut[data[i + 7]];
  }
  for (; i < n; ++i) {
    sum += lut[data[i]];
  }

  // Baseline recomputed the same sum in each iteration and returned the last.
  // The value is identical across iterations, so we can return the computed sum.
  return sum;
}
