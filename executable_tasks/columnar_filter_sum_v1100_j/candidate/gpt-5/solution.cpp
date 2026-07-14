#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = keys.size();
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & ~std::size_t(3);
  for (; i < n4; i += 4) {
    // 0
    {
      uint8_t f = flags[i];
      if (f) {
        uint32_t k = keys[i];
        if (k < key_threshold) {
          if ((k & 3u) == 0u) {
            s0 += static_cast<uint64_t>(values[i]);
          }
        }
      }
    }
    // 1
    {
      uint8_t f = flags[i + 1];
      if (f) {
        uint32_t k = keys[i + 1];
        if (k < key_threshold) {
          if ((k & 3u) == 0u) {
            s1 += static_cast<uint64_t>(values[i + 1]);
          }
        }
      }
    }
    // 2
    {
      uint8_t f = flags[i + 2];
      if (f) {
        uint32_t k = keys[i + 2];
        if (k < key_threshold) {
          if ((k & 3u) == 0u) {
            s2 += static_cast<uint64_t>(values[i + 2]);
          }
        }
      }
    }
    // 3
    {
      uint8_t f = flags[i + 3];
      if (f) {
        uint32_t k = keys[i + 3];
        if (k < key_threshold) {
          if ((k & 3u) == 0u) {
            s3 += static_cast<uint64_t>(values[i + 3]);
          }
        }
      }
    }
  }

  for (; i < n; ++i) {
    uint8_t f = flags[i];
    if (f) {
      uint32_t k = keys[i];
      if (k < key_threshold) {
        if ((k & 3u) == 0u) {
          sum += static_cast<uint64_t>(values[i]);
        }
      }
    }
  }

  sum += s0 + s1 + s2 + s3;
  return sum;
}
