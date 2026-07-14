#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const uint8_t* data = packed.data();
  const size_t n = packed.size();
  const uint8_t thr = threshold;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0;

    // Unroll by 8 bytes for improved throughput
    size_t n_unroll = n & ~static_cast<size_t>(7);
    for (; i < n_unroll; i += 8) {
      uint8_t b0 = data[i + 0];
      uint8_t b1 = data[i + 1];
      uint8_t b2 = data[i + 2];
      uint8_t b3 = data[i + 3];
      uint8_t b4 = data[i + 4];
      uint8_t b5 = data[i + 5];
      uint8_t b6 = data[i + 6];
      uint8_t b7 = data[i + 7];

      uint8_t lo0 = static_cast<uint8_t>(b0 & 0x0F); uint8_t hi0 = static_cast<uint8_t>(b0 >> 4);
      uint8_t lo1 = static_cast<uint8_t>(b1 & 0x0F); uint8_t hi1 = static_cast<uint8_t>(b1 >> 4);
      uint8_t lo2 = static_cast<uint8_t>(b2 & 0x0F); uint8_t hi2 = static_cast<uint8_t>(b2 >> 4);
      uint8_t lo3 = static_cast<uint8_t>(b3 & 0x0F); uint8_t hi3 = static_cast<uint8_t>(b3 >> 4);
      uint8_t lo4 = static_cast<uint8_t>(b4 & 0x0F); uint8_t hi4 = static_cast<uint8_t>(b4 >> 4);
      uint8_t lo5 = static_cast<uint8_t>(b5 & 0x0F); uint8_t hi5 = static_cast<uint8_t>(b5 >> 4);
      uint8_t lo6 = static_cast<uint8_t>(b6 & 0x0F); uint8_t hi6 = static_cast<uint8_t>(b6 >> 4);
      uint8_t lo7 = static_cast<uint8_t>(b7 & 0x0F); uint8_t hi7 = static_cast<uint8_t>(b7 >> 4);

      sum += (static_cast<uint64_t>(lo0) & (-(uint64_t)(lo0 > thr)));
      sum += (static_cast<uint64_t>(hi0) & (-(uint64_t)(hi0 > thr)));
      sum += (static_cast<uint64_t>(lo1) & (-(uint64_t)(lo1 > thr)));
      sum += (static_cast<uint64_t>(hi1) & (-(uint64_t)(hi1 > thr)));
      sum += (static_cast<uint64_t>(lo2) & (-(uint64_t)(lo2 > thr)));
      sum += (static_cast<uint64_t>(hi2) & (-(uint64_t)(hi2 > thr)));
      sum += (static_cast<uint64_t>(lo3) & (-(uint64_t)(lo3 > thr)));
      sum += (static_cast<uint64_t>(hi3) & (-(uint64_t)(hi3 > thr)));
      sum += (static_cast<uint64_t>(lo4) & (-(uint64_t)(lo4 > thr)));
      sum += (static_cast<uint64_t>(hi4) & (-(uint64_t)(hi4 > thr)));
      sum += (static_cast<uint64_t>(lo5) & (-(uint64_t)(lo5 > thr)));
      sum += (static_cast<uint64_t>(hi5) & (-(uint64_t)(hi5 > thr)));
      sum += (static_cast<uint64_t>(lo6) & (-(uint64_t)(lo6 > thr)));
      sum += (static_cast<uint64_t>(hi6) & (-(uint64_t)(hi6 > thr)));
      sum += (static_cast<uint64_t>(lo7) & (-(uint64_t)(lo7 > thr)));
      sum += (static_cast<uint64_t>(hi7) & (-(uint64_t)(hi7 > thr)));
    }

    for (; i < n; ++i) {
      uint8_t b = data[i];
      uint8_t lo = static_cast<uint8_t>(b & 0x0F);
      uint8_t hi = static_cast<uint8_t>(b >> 4);
      sum += (static_cast<uint64_t>(lo) & (-(uint64_t)(lo > thr)));
      sum += (static_cast<uint64_t>(hi) & (-(uint64_t)(hi > thr)));
    }
  }

  return sum;
}
