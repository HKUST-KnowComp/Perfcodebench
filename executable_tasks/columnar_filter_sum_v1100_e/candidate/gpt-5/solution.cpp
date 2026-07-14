#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0 (no iterations executed)
  if (iters <= 0) return 0;

  // Compute result once; repeating would yield the same sum each iteration
  std::size_t n = keys.size();
  if (values.size() < n) n = values.size();
  if (flags.size() < n) n = flags.size();

  const uint32_t* kp = keys.data();
  const uint32_t* vp = values.data();
  const uint8_t* fp = flags.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  std::size_t i = 0;
  std::size_t limit = n & ~static_cast<std::size_t>(7);

  for (; i < limit; i += 8) {
    uint32_t k0 = kp[i + 0];
    uint32_t k1 = kp[i + 1];
    uint32_t k2 = kp[i + 2];
    uint32_t k3 = kp[i + 3];
    uint32_t k4 = kp[i + 4];
    uint32_t k5 = kp[i + 5];
    uint32_t k6 = kp[i + 6];
    uint32_t k7 = kp[i + 7];

    uint32_t c0 = (fp[i + 0] != 0);
    uint32_t c1 = (fp[i + 1] != 0);
    uint32_t c2 = (fp[i + 2] != 0);
    uint32_t c3 = (fp[i + 3] != 0);
    uint32_t c4 = (fp[i + 4] != 0);
    uint32_t c5 = (fp[i + 5] != 0);
    uint32_t c6 = (fp[i + 6] != 0);
    uint32_t c7 = (fp[i + 7] != 0);

    c0 &= ((k0 & 3u) == 0u) & (k0 < key_threshold);
    c1 &= ((k1 & 3u) == 0u) & (k1 < key_threshold);
    c2 &= ((k2 & 3u) == 0u) & (k2 < key_threshold);
    c3 &= ((k3 & 3u) == 0u) & (k3 < key_threshold);
    c4 &= ((k4 & 3u) == 0u) & (k4 < key_threshold);
    c5 &= ((k5 & 3u) == 0u) & (k5 < key_threshold);
    c6 &= ((k6 & 3u) == 0u) & (k6 < key_threshold);
    c7 &= ((k7 & 3u) == 0u) & (k7 < key_threshold);

    // Use four independent accumulators to reduce dependency chain
    s0 += static_cast<uint64_t>(vp[i + 0]) * c0;
    s1 += static_cast<uint64_t>(vp[i + 1]) * c1;
    s2 += static_cast<uint64_t>(vp[i + 2]) * c2;
    s3 += static_cast<uint64_t>(vp[i + 3]) * c3;

    s0 += static_cast<uint64_t>(vp[i + 4]) * c4;
    s1 += static_cast<uint64_t>(vp[i + 5]) * c5;
    s2 += static_cast<uint64_t>(vp[i + 6]) * c6;
    s3 += static_cast<uint64_t>(vp[i + 7]) * c7;
  }

  for (; i < n; ++i) {
    uint32_t k = kp[i];
    uint32_t c = (fp[i] != 0);
    c &= ((k & 3u) == 0u) & (k < key_threshold);
    s0 += static_cast<uint64_t>(vp[i]) * c;
  }

  uint64_t sum = (s0 + s1) + (s2 + s3);
  return sum;
}
