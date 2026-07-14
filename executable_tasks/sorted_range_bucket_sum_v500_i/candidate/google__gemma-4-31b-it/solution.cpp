#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0 || probes.empty()) {
    return 0;
  }

  uint64_t single_pass_sum = 0;
  size_t b_idx = 0;
  size_t b_size = boundaries.size();

  for (uint32_t probe : probes) {
    // Since probes are sorted, we only need to move the boundary index forward
    while (b_idx < b_size && boundaries[b_idx] <= probe) {
      b_idx++;
    }
    single_pass_sum += b_idx;
  }

  return single_pass_sum * iters;
}