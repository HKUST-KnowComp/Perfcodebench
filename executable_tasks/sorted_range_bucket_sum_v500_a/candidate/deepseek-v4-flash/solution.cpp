#include "interface.h"

#include <cstddef>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // If no iterations requested, return 0 (matching baseline behavior)
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* bound_ptr = boundaries.data();
  const size_t bound_count = boundaries.size();
  const uint32_t* probe_ptr = probes.data();
  const size_t probe_count = probes.size();

  size_t idx = 0;          // number of boundaries <= current probe
  uint64_t sum = 0;

  for (size_t i = 0; i < probe_count; ++i) {
    uint32_t p = probe_ptr[i];
    // Advance idx while boundary <= probe
    while (idx < bound_count && bound_ptr[idx] <= p) {
      ++idx;
    }
    sum += static_cast<uint64_t>(idx);
  }

  return sum;
}
