#include "interface.h"

#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* kp = keys.data();
  const uint32_t* vp = values.data();
  const uint8_t* fp = flags.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);

    for (; i < n4; i += 4) {
      uint32_t k0 = kp[i];
      uint32_t k1 = kp[i + 1];
      uint32_t k2 = kp[i + 2];
      uint32_t k3 = kp[i + 3];

      uint32_t v0 = vp[i];
      uint32_t v1 = vp[i + 1];
      uint32_t v2 = vp[i + 2];
      uint32_t v3 = vp[i + 3];

      uint8_t f0 = fp[i];
      uint8_t f1 = fp[i + 1];
      uint8_t f2 = fp[i + 2];
      uint8_t f3 = fp[i + 3];

      unsigned c0 = (f0 != 0) & (k0 < key_threshold) & ((k0 & 3u) == 0u);
      unsigned c1 = (f1 != 0) & (k1 < key_threshold) & ((k1 & 3u) == 0u);
      unsigned c2 = (f2 != 0) & (k2 < key_threshold) & ((k2 & 3u) == 0u);
      unsigned c3 = (f3 != 0) & (k3 < key_threshold) & ((k3 & 3u) == 0u);

      uint32_t m0 = static_cast<uint32_t>(-static_cast<int>(c0));
      uint32_t m1 = static_cast<uint32_t>(-static_cast<int>(c1));
      uint32_t m2 = static_cast<uint32_t>(-static_cast<int>(c2));
      uint32_t m3 = static_cast<uint32_t>(-static_cast<int>(c3));

      s0 += static_cast<uint64_t>(v0 & m0);
      s1 += static_cast<uint64_t>(v1 & m1);
      s2 += static_cast<uint64_t>(v2 & m2);
      s3 += static_cast<uint64_t>(v3 & m3);
    }

    uint64_t s = s0 + s1 + s2 + s3;

    for (; i < n; ++i) {
      uint32_t k = kp[i];
      uint32_t v = vp[i];
      unsigned c = (fp[i] != 0) & (k < key_threshold) & ((k & 3u) == 0u);
      uint32_t m = static_cast<uint32_t>(-static_cast<int>(c));
      s += static_cast<uint64_t>(v & m);
    }

    sum = s;
  }

  return sum;
}
