#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict k_ptr = keys.data();
  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict p_ptr = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint32_t k0 = k_ptr[i];
      uint32_t k1 = k_ptr[i + 1];
      uint32_t k2 = k_ptr[i + 2];
      uint32_t k3 = k_ptr[i + 3];
      uint8_t f0 = f_ptr[i];
      uint8_t f1 = f_ptr[i + 1];
      uint8_t f2 = f_ptr[i + 2];
      uint8_t f3 = f_ptr[i + 3];
      bool c0 = f0 && (k0 < threshold) && ((k0 & 7u) == 0u);
      bool c1 = f1 && (k1 < threshold) && ((k1 & 7u) == 0u);
      bool c2 = f2 && (k2 < threshold) && ((k2 & 7u) == 0u);
      bool c3 = f3 && (k3 < threshold) && ((k3 & 7u) == 0u);
      s0 += c0 ? static_cast<uint64_t>(p_ptr[i]) : 0;
      s1 += c1 ? static_cast<uint64_t>(p_ptr[i + 1]) : 0;
      s2 += c2 ? static_cast<uint64_t>(p_ptr[i + 2]) : 0;
      s3 += c3 ? static_cast<uint64_t>(p_ptr[i + 3]) : 0;
    }
    for (; i < n; ++i) {
      uint32_t k = k_ptr[i];
      if (f_ptr[i] && (k < threshold) && ((k & 7u) == 0u)) {
        s0 += static_cast<uint64_t>(p_ptr[i]);
      }
    }
    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}
