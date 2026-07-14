#include "interface.h"
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters)
{
  const std::size_t n = deltas.size();
  uint64_t total = 0;

  if (low == 0 && high == 0xFFFFFFFFU) {
    // fast path: every value is accepted
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(base + deltas[i]);
      }
      total = sum;
    }
    return total;
  }

  if (low == 0) {
    // unswitch: only upper bound check
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        uint32_t v = base + deltas[i];
        if (v <= high) sum += v;
      }
      total = sum;
    }
    return total;
  }

  if (high == 0xFFFFFFFFU) {
    // unswitch: only lower bound check
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        uint32_t v = base + deltas[i];
        if (v >= low) sum += v;
      }
      total = sum;
    }
    return total;
  }

  // general path
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t v = base + deltas[i];
      if (v >= low && v <= high) sum += v;
    }
    total = sum;
  }
  return total;
}