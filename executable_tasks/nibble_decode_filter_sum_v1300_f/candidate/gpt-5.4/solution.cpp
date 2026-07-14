#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // Values are 4-bit, so thresholds >= 15 can never pass.
  if (threshold >= 15) {
    return 0;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  const uint8_t t = threshold;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n & ~(unroll - 1);

    for (; i < limit; i += unroll) {
      uint8_t b0 = data[i + 0];
      uint8_t b1 = data[i + 1];
      uint8_t b2 = data[i + 2];
      uint8_t b3 = data[i + 3];
      uint8_t b4 = data[i + 4];
      uint8_t b5 = data[i + 5];
      uint8_t b6 = data[i + 6];
      uint8_t b7 = data[i + 7];

      uint8_t v;

      v = static_cast<uint8_t>(b0 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b0 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b1 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b1 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b2 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b2 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b3 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b3 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b4 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b4 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b5 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b5 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b6 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b6 >> 4);    if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b7 & 0x0Fu); if (v > t) local_sum += v;
      v = static_cast<uint8_t>(b7 >> 4);    if (v > t) local_sum += v;
    }

    for (; i < n; ++i) {
      uint8_t b = data[i];
      uint8_t lo = static_cast<uint8_t>(b & 0x0Fu);
      uint8_t hi = static_cast<uint8_t>(b >> 4);
      if (lo > t) local_sum += lo;
      if (hi > t) local_sum += hi;
    }

    sum = local_sum;
  }

  return sum;
}
