#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  // Preserve baseline behavior for non-positive iters
  if (iters <= 0) {
    return 0ull;
  }

  // Empty range fast-path
  if (high < low) {
    return 0ull;
  }

  const size_t n = values.size();
  const uint32_t* v = n ? values.data() : nullptr;

  // If no grouping or metadata unusable, do a full scan once
  auto full_scan = [&]() {
    uint64_t sum = 0ull;
    for (size_t i = 0; i < n; ++i) {
      uint32_t val = v[i];
      if (val >= low && val <= high) {
        sum += static_cast<uint64_t>(val);
      }
    }
    return sum;
  };

  if (block_size == 0) {
    return full_scan();
  }

  const size_t blocks = (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);
  if (mins.size() < blocks || maxs.size() < blocks) {
    // Metadata size mismatch: safest is a full scan
    return full_scan();
  }

  uint64_t sum = 0ull;

  // Iterate blocks using pruning and full-inclusion checks
  for (size_t b = 0; b < blocks; ++b) {
    const uint32_t bmin = mins[b];
    const uint32_t bmax = maxs[b];

    // No overlap with [low, high]
    if (bmax < low || bmin > high) {
      continue;
    }

    // Determine block bounds
    const size_t start = b * static_cast<size_t>(block_size);
    size_t end = start + static_cast<size_t>(block_size);
    if (end > n) end = n;

    // Fully included: sum all values in block without per-value checks
    if (bmin >= low && bmax <= high) {
      // Unrolled summation for throughput
      size_t i = start;
      uint64_t local = 0ull;
      for (; i + 7 < end; i += 8) {
        local += v[i + 0];
        local += v[i + 1];
        local += v[i + 2];
        local += v[i + 3];
        local += v[i + 4];
        local += v[i + 5];
        local += v[i + 6];
        local += v[i + 7];
      }
      for (; i < end; ++i) {
        local += v[i];
      }
      sum += local;
    } else {
      // Partial overlap: filter per value
      for (size_t i = start; i < end; ++i) {
        uint32_t val = v[i];
        if (val >= low && val <= high) {
          sum += static_cast<uint64_t>(val);
        }
      }
    }
  }

  return sum;
}
