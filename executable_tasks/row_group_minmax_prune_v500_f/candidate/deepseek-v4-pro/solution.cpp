#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  const size_t n = values.size();
  const size_t num_groups = mins.size();

  // Precompute ranges of full-cover and partial-cover blocks
  std::vector<size_t> full_start, full_end;
  std::vector<size_t> partial_start, partial_end;

  for (size_t i = 0; i < num_groups; ++i) {
    // Skip groups that cannot contain any matching value
    if (maxs[i] < low || mins[i] > high) continue;

    size_t start = i * block_size;
    size_t end = std::min(start + block_size, n);

    if (mins[i] >= low && maxs[i] <= high) {
      // Entire block qualifies – no per‑value filter needed
      full_start.push_back(start);
      full_end.push_back(end);
    } else {
      // Block partially overlaps – must check each value
      partial_start.push_back(start);
      partial_end.push_back(end);
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    // Sum fully‑covered blocks (fast, no branches)
    for (size_t k = 0; k < full_start.size(); ++k) {
      const size_t s = full_start[k];
      const size_t e = full_end[k];
      for (size_t j = s; j < e; ++j) {
        sum += static_cast<uint64_t>(values[j]);
      }
    }

    // Sum partially‑covered blocks (filter required)
    for (size_t k = 0; k < partial_start.size(); ++k) {
      const size_t s = partial_start[k];
      const size_t e = partial_end[k];
      for (size_t j = s; j < e; ++j) {
        const uint32_t v = values[j];
        if (v >= low && v <= high) {
          sum += static_cast<uint64_t>(v);
        }
      }
    }
  }

  return sum;
}