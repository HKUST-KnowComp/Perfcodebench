#include "interface.h"

static inline uint64_t sum_block_all(const uint32_t* p, size_t n) {
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  size_t i = 0;
  size_t n4 = n & ~size_t(3);
  for (; i < n4; i += 4) {
    s0 += p[i + 0];
    s1 += p[i + 1];
    s2 += p[i + 2];
    s3 += p[i + 3];
  }
  uint64_t s = s0 + s1 + s2 + s3;
  for (; i < n; ++i) s += p[i];
  return s;
}

static inline uint64_t sum_block_range(const uint32_t* p, size_t n, uint32_t low, uint32_t high) {
  // Assumes high >= low
  const uint32_t span = high - low;
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  size_t i = 0;
  size_t n4 = n & ~size_t(3);
  for (; i < n4; i += 4) {
    uint32_t v0 = p[i + 0];
    uint32_t v1 = p[i + 1];
    uint32_t v2 = p[i + 2];
    uint32_t v3 = p[i + 3];
    if ((uint32_t)(v0 - low) <= span) s0 += v0;
    if ((uint32_t)(v1 - low) <= span) s1 += v1;
    if ((uint32_t)(v2 - low) <= span) s2 += v2;
    if ((uint32_t)(v3 - low) <= span) s3 += v3;
  }
  uint64_t s = s0 + s1 + s2 + s3;
  for (; i < n; ++i) {
    uint32_t v = p[i];
    if ((uint32_t)(v - low) <= span) s += v;
  }
  return s;
}

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int /*iters*/) {
  const size_t n = values.size();
  if (n == 0) return 0ULL;
  if (high < low) return 0ULL; // Empty range

  const uint32_t* data = values.data();

  // Fallback to full scan if metadata unusable
  if (mins.empty() || maxs.empty() || block_size == 0) {
    return sum_block_range(data, n, low, high);
  }

  uint64_t total = 0;
  const size_t nblocks = mins.size();

  for (size_t b = 0; b < nblocks; ++b) {
    size_t start = b * (size_t)block_size;
    if (start >= n) break; // No more data
    size_t end = start + (size_t)block_size;
    if (end > n) end = n;
    size_t len = end - start;

    uint32_t bmin = mins[b];
    uint32_t bmax = maxs[b];

    // Prune blocks completely outside the range
    if (bmax < low || bmin > high) continue;

    // Fully contained block: add all values without per-element checks
    if (bmin >= low && bmax <= high) {
      total += sum_block_all(data + start, len);
    } else {
      // Partial overlap: check each value
      total += sum_block_range(data + start, len, low, high);
    }
  }

  return total;
}
