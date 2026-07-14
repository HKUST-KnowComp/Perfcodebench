#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0 || boundaries.empty() || probes.empty()) {
    return 0;
  }
  uint64_t per_iter_sum = 0;
  size_t bound_idx = 0;
  const size_t bound_count = boundaries.size();
  for (uint32_t probe : probes) {
    while (bound_idx < bound_count && boundaries[bound_idx] <= probe) {
      ++bound_idx;
    }
    per_iter_sum += bound_idx;
  }
  return per_iter_sum * static_cast<uint64_t>(iters);
}