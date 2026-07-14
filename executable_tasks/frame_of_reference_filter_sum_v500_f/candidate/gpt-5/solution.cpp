#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int /*iters*/) {
  // If the range is empty, no values can match
  if (high < low || deltas.empty()) {
    return 0;
  }

  const uint16_t* data = deltas.data();
  const std::size_t n = deltas.size();

  const uint32_t base32 = base;
  const uint32_t low32 = low;
  const uint32_t bound = high - low; // safe since high >= low

  uint64_t sum = 0;

  std::size_t i = 0;
  // Unroll by 8 for better instruction throughput
  for (; i + 8 <= n; i += 8) {
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 0]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 1]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 2]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 3]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 4]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 5]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 6]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
    {
      uint32_t v = base32 + static_cast<uint32_t>(data[i + 7]);
      uint32_t r = v - low32;
      sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
    }
  }

  // Remainder
  for (; i < n; ++i) {
    uint32_t v = base32 + static_cast<uint32_t>(data[i]);
    uint32_t r = v - low32;
    sum += static_cast<uint64_t>((r <= bound) ? v : 0u);
  }

  return sum;
}
