#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;
  const size_t n = keys.size();
  const uint32_t* kp = keys.data();
  const uint8_t* fp = flags.data();
  const uint32_t* pp = payloads.data();
  uint64_t sum = 0;
  for (size_t i = 0; i < n; ++i) {
    bool ok = (fp[i] != 0) && (kp[i] < threshold) && ((kp[i] & 7u) == 0u);
    sum += static_cast<uint64_t>(ok) * static_cast<uint64_t>(pp[i]);
  }
  return sum;
}