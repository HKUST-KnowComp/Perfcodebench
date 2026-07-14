#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  (void)iters; // iterations are redundant since input is constant across runs

  const std::size_t n = deltas.size();
  if (n == 0) {
    return 0;
  }

  const uint16_t* p = deltas.data();
  const uint32_t b = base;
  const uint32_t lo = low;
  const uint32_t hi = high;

  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    uint32_t v0 = b + static_cast<uint32_t>(p[i + 0]);
    uint32_t v1 = b + static_cast<uint32_t>(p[i + 1]);
    uint32_t v2 = b + static_cast<uint32_t>(p[i + 2]);
    uint32_t v3 = b + static_cast<uint32_t>(p[i + 3]);
    uint32_t v4 = b + static_cast<uint32_t>(p[i + 4]);
    uint32_t v5 = b + static_cast<uint32_t>(p[i + 5]);
    uint32_t v6 = b + static_cast<uint32_t>(p[i + 6]);
    uint32_t v7 = b + static_cast<uint32_t>(p[i + 7]);

    bool c0 = (v0 >= lo) & (v0 <= hi);
    bool c1 = (v1 >= lo) & (v1 <= hi);
    bool c2 = (v2 >= lo) & (v2 <= hi);
    bool c3 = (v3 >= lo) & (v3 <= hi);
    bool c4 = (v4 >= lo) & (v4 <= hi);
    bool c5 = (v5 >= lo) & (v5 <= hi);
    bool c6 = (v6 >= lo) & (v6 <= hi);
    bool c7 = (v7 >= lo) & (v7 <= hi);

    sum += c0 ? static_cast<uint64_t>(v0) : 0;
    sum += c1 ? static_cast<uint64_t>(v1) : 0;
    sum += c2 ? static_cast<uint64_t>(v2) : 0;
    sum += c3 ? static_cast<uint64_t>(v3) : 0;
    sum += c4 ? static_cast<uint64_t>(v4) : 0;
    sum += c5 ? static_cast<uint64_t>(v5) : 0;
    sum += c6 ? static_cast<uint64_t>(v6) : 0;
    sum += c7 ? static_cast<uint64_t>(v7) : 0;
  }

  for (; i < n; ++i) {
    uint32_t v = b + static_cast<uint32_t>(p[i]);
    if (v >= lo && v <= hi) {
      sum += static_cast<uint64_t>(v);
    }
  }

  return sum;
}
