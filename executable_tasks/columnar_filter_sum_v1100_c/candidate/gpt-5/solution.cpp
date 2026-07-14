#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int /*iters*/) {
  const std::size_t n = keys.size();
  if (n == 0) return 0;

  const uint32_t* kp = keys.data();
  const uint32_t* vp = values.data();
  const uint8_t* fp = flags.data();

  uint64_t sum = 0;

  #if defined(__GNUC__) || defined(__clang__)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
  #else
    #define UNLIKELY(x) (x)
  #endif

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  for (; i < n4; i += 4) {
    const uint32_t k0 = kp[i+0];
    const uint32_t k1 = kp[i+1];
    const uint32_t k2 = kp[i+2];
    const uint32_t k3 = kp[i+3];

    const uint8_t f0 = fp[i+0];
    const uint8_t f1 = fp[i+1];
    const uint8_t f2 = fp[i+2];
    const uint8_t f3 = fp[i+3];

    if (UNLIKELY(f0 && ((k0 & 3u) == 0u) && (k0 < key_threshold))) sum += static_cast<uint64_t>(vp[i+0]);
    if (UNLIKELY(f1 && ((k1 & 3u) == 0u) && (k1 < key_threshold))) sum += static_cast<uint64_t>(vp[i+1]);
    if (UNLIKELY(f2 && ((k2 & 3u) == 0u) && (k2 < key_threshold))) sum += static_cast<uint64_t>(vp[i+2]);
    if (UNLIKELY(f3 && ((k3 & 3u) == 0u) && (k3 < key_threshold))) sum += static_cast<uint64_t>(vp[i+3]);
  }

  for (; i < n; ++i) {
    const uint32_t k = kp[i];
    if (UNLIKELY(fp[i] && ((k & 3u) == 0u) && (k < key_threshold))) {
      sum += static_cast<uint64_t>(vp[i]);
    }
  }

  return sum;
}
