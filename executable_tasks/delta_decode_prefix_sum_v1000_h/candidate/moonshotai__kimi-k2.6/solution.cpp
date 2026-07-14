#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters <= 0) return 0;
  const uint64_t n = static_cast<uint64_t>(deltas.size());
  uint64_t weighted = 0;
  for (uint64_t i = 0; i < n; ++i) {
    weighted += static_cast<uint64_t>(deltas[i]) * (n - i);
  }
  const uint64_t per_iter = n * static_cast<uint64_t>(base) + weighted;
  return per_iter * static_cast<uint64_t>(iters);
}
