#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t*  f = flags.data();
  const std::size_t n = keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc = 0;
    std::size_t i = 0;

    // Peel off groups of 4 to reduce loop overhead and expose ILP
    for (; i + 3 < n; i += 4) {
      uint32_t k0 = k[i + 0];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];

      uint32_t m0 = (f[i + 0] & ((k0 < key_threshold) & ((k0 & 3u) == 0u)));
      uint32_t m1 = (f[i + 1] & ((k1 < key_threshold) & ((k1 & 3u) == 0u)));
      uint32_t m2 = (f[i + 2] & ((k2 < key_threshold) & ((k2 & 3u) == 0u)));
      uint32_t m3 = (f[i + 3] & ((k3 < key_threshold) & ((k3 & 3u) == 0u)));

      acc += uint64_t{v[i + 0]} * m0;
      acc += uint64_t{v[i + 1]} * m1;
      acc += uint64_t{v[i + 2]} * m2;
      acc += uint64_t{v[i + 3]} * m3;
    }

    // Remainder
    for (; i < n; ++i) {
      uint32_t ki = k[i];
      uint32_t mask = f[i] & ((ki < key_threshold) & ((ki & 3u) == 0u));
      acc += uint64_t{v[i]} * mask;
    }

    sum = acc;
  }
  return sum;
}