#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Match baseline behavior for non-positive iters
  if (iters <= 0) {
    return 0;
  }

  const size_t nb = boundaries.size();
  const size_t np = probes.size();

  if (np == 0 || nb == 0) {
    return 0;
  }

  const uint32_t* bp = boundaries.data();
  const uint32_t* pp = probes.data();

  uint64_t sum = 0;
  size_t i = 0; // index into boundaries: count of <= current probe
  size_t j = 0; // index into probes

  while (j < np) {
    const uint32_t p = pp[j];
    // Advance boundary index to first element > p (upper_bound behavior)
    while (i < nb && bp[i] <= p) {
      ++i;
    }
    if (i == nb) {
      // All remaining probes map to bucket index nb
      sum += static_cast<uint64_t>(nb) * static_cast<uint64_t>(np - j);
      break;
    }
    // Aggregate runs of equal probe values
    size_t k = j + 1;
    while (k < np && pp[k] == p) {
      ++k;
    }
    sum += static_cast<uint64_t>(i) * static_cast<uint64_t>(k - j);
    j = k;
  }

  return sum;
}
