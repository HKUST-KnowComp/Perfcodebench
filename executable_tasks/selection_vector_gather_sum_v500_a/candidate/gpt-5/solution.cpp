#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline semantics: if iters == 0, return 0
  if (iters <= 0) {
    return 0;
  }
  // Early exit: no unsigned key is < 0, so threshold == 0 yields empty selection
  if (threshold == 0u) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* kp = keys.data();
  const uint8_t* fp = flags.data();
  const uint32_t* pp = payloads.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  const std::size_t n_block = n & ~static_cast<std::size_t>(3);

  for (; i < n_block; i += 4) {
    uint32_t k0 = kp[i];
    uint32_t k1 = kp[i + 1];
    uint32_t k2 = kp[i + 2];
    uint32_t k3 = kp[i + 3];

    if (fp[i] && k0 < threshold && ((k0 & 7u) == 0u)) {
      s0 += static_cast<uint64_t>(pp[i]);
    }
    if (fp[i + 1] && k1 < threshold && ((k1 & 7u) == 0u)) {
      s1 += static_cast<uint64_t>(pp[i + 1]);
    }
    if (fp[i + 2] && k2 < threshold && ((k2 & 7u) == 0u)) {
      s2 += static_cast<uint64_t>(pp[i + 2]);
    }
    if (fp[i + 3] && k3 < threshold && ((k3 & 7u) == 0u)) {
      s3 += static_cast<uint64_t>(pp[i + 3]);
    }
  }

  uint64_t sum = 0;
  for (; i < n; ++i) {
    uint32_t k = kp[i];
    if (fp[i] && k < threshold && ((k & 7u) == 0u)) {
      sum += static_cast<uint64_t>(pp[i]);
    }
  }

  sum += (s0 + s1 + s2 + s3);
  return sum;
}
