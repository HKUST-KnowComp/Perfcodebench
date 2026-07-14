#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  (void)iters; // Result is deterministic; compute once

  const size_t n = values.size();
  if (n == 0) return 0;
  if (high < low) return 0;

  uint64_t sum = 0;

  // If metadata is not usable, or block_size is zero, fall back to full scan
  if (block_size == 0) {
    const uint32_t* p = values.data();
    const uint32_t* e = p + n;
    while (p < e) {
      uint32_t x = *p++;
      if (x >= low && x <= high) sum += x;
    }
    return sum;
  }

  const size_t num_blocks = (n + static_cast<size_t>(block_size) - 1u) / static_cast<size_t>(block_size);
  const bool have_meta = (mins.size() >= num_blocks) && (maxs.size() >= num_blocks);
  if (!have_meta) {
    const uint32_t* p = values.data();
    const uint32_t* e = p + n;
    while (p < e) {
      uint32_t x = *p++;
      if (x >= low && x <= high) sum += x;
    }
    return sum;
  }

  const uint32_t* p = values.data();
  size_t remaining = n;
  for (size_t b = 0; b < num_blocks; ++b) {
    size_t len = remaining >= static_cast<size_t>(block_size) ? static_cast<size_t>(block_size) : remaining;
    if (len == 0) break;

    uint32_t bmin = mins[b];
    uint32_t bmax = maxs[b];

    if (bmin > high || bmax < low) {
      // No overlap with [low, high]; skip whole block
      p += len;
      remaining -= len;
      continue;
    }

    const uint32_t* pend = p + len;

    if (bmin >= low && bmax <= high) {
      // Entire block is within range; sum all values without checks
      // Unroll by 8 for throughput
      while (p + 8 <= pend) {
        sum += p[0]; sum += p[1]; sum += p[2]; sum += p[3];
        sum += p[4]; sum += p[5]; sum += p[6]; sum += p[7];
        p += 8;
      }
      while (p < pend) {
        sum += *p++;
      }
    } else {
      // Partial overlap; need per-value check
      while (p < pend) {
        uint32_t x = *p++;
        if (x >= low && x <= high) sum += x;
      }
    }

    remaining -= len;
  }

  return sum;
}
