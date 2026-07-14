#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Fast path: 12-bit values are in [0, 4095]. If threshold >= 4095, no value can be > threshold.
  if (threshold >= 4095) {
    return 0ull;
  }

  uint64_t sum = 0;
  const std::size_t nbytes = packed.size();
  const uint8_t* data = packed.data();

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t thr = static_cast<uint32_t>(threshold);
    const uint8_t* p = data;
    const uint8_t* const end = data + nbytes;
    uint64_t s = 0ull;

    // Unroll by 4 pairs (12 bytes) at a time
    const std::size_t unroll_bytes = 12u;
    const uint8_t* const end_unroll = data + (nbytes / unroll_bytes) * unroll_bytes;

    while (p < end_unroll) {
      // Pair 1
      uint32_t b0 = p[0];
      uint32_t b1 = p[1];
      uint32_t b2 = p[2];
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      if (a > thr) s += a;
      if (b > thr) s += b;

      // Pair 2
      b0 = p[3];
      b1 = p[4];
      b2 = p[5];
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) s += a;
      if (b > thr) s += b;

      // Pair 3
      b0 = p[6];
      b1 = p[7];
      b2 = p[8];
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) s += a;
      if (b > thr) s += b;

      // Pair 4
      b0 = p[9];
      b1 = p[10];
      b2 = p[11];
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) s += a;
      if (b > thr) s += b;

      p += 12;
    }

    // Handle remaining full pairs (3-byte groups)
    const uint8_t* const end_pairs = p + ((end - p) / 3u) * 3u;
    while (p < end_pairs) {
      uint32_t b0 = p[0];
      uint32_t b1 = p[1];
      uint32_t b2 = p[2];
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      if (a > thr) s += a;
      if (b > thr) s += b;
      p += 3;
    }

    sum = s; // per contract: return sum from the last iteration
  }

  return sum;
}
