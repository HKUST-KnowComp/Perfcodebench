#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();
  const uint32_t thresh = key_threshold;

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    uint32_t k0 = k[i+0]; if (f[i+0] && k0 < thresh && ((k0 & 3u) == 0u)) s0 += static_cast<uint64_t>(v[i+0]);
    uint32_t k1 = k[i+1]; if (f[i+1] && k1 < thresh && ((k1 & 3u) == 0u)) s1 += static_cast<uint64_t>(v[i+1]);
    uint32_t k2 = k[i+2]; if (f[i+2] && k2 < thresh && ((k2 & 3u) == 0u)) s2 += static_cast<uint64_t>(v[i+2]);
    uint32_t k3 = k[i+3]; if (f[i+3] && k3 < thresh && ((k3 & 3u) == 0u)) s3 += static_cast<uint64_t>(v[i+3]);
    uint32_t k4 = k[i+4]; if (f[i+4] && k4 < thresh && ((k4 & 3u) == 0u)) s4 += static_cast<uint64_t>(v[i+4]);
    uint32_t k5 = k[i+5]; if (f[i+5] && k5 < thresh && ((k5 & 3u) == 0u)) s5 += static_cast<uint64_t>(v[i+5]);
    uint32_t k6 = k[i+6]; if (f[i+6] && k6 < thresh && ((k6 & 3u) == 0u)) s6 += static_cast<uint64_t>(v[i+6]);
    uint32_t k7 = k[i+7]; if (f[i+7] && k7 < thresh && ((k7 & 3u) == 0u)) s7 += static_cast<uint64_t>(v[i+7]);
  }
  for (; i < n; ++i) {
    uint32_t ki = k[i];
    if (f[i] && ki < thresh && ((ki & 3u) == 0u)) {
      s0 += static_cast<uint64_t>(v[i]);
    }
  }

  uint64_t sum = (((s0 + s1) + (s2 + s3)) + ((s4 + s5) + (s6 + s7)));
  return sum;
}
