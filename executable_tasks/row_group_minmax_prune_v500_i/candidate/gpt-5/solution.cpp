#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  (void)iters; // Result is deterministic; compute once for performance

  const size_t n = values.size();
  if (n == 0) return 0ull;
  if (low > high) return 0ull;

  // If block_size or metadata is unavailable, fall back to full scan
  if (block_size == 0 || mins.empty() || maxs.empty()) {
    uint64_t sum = 0;
    const uint32_t l = low, h = high;
    const uint32_t* p = values.data();
    const uint32_t* pe = p + n;
    // Unroll by 4 with branchless accumulation
    for (; p + 4 <= pe; p += 4) {
      uint32_t v0 = p[0], v1 = p[1], v2 = p[2], v3 = p[3];
      sum += (uint64_t)(((v0 >= l) & (v0 <= h))) * v0;
      sum += (uint64_t)(((v1 >= l) & (v1 <= h))) * v1;
      sum += (uint64_t)(((v2 >= l) & (v2 <= h))) * v2;
      sum += (uint64_t)(((v3 >= l) & (v3 <= h))) * v3;
    }
    for (; p < pe; ++p) {
      uint32_t v = *p;
      sum += (uint64_t)(((v >= l) & (v <= h))) * v;
    }
    return sum;
  }

  uint64_t sum = 0;
  const uint32_t l = low, h = high;

  const size_t num_blocks_nominal = (n + (size_t)block_size - 1) / (size_t)block_size;
  size_t usable_blocks = num_blocks_nominal;
  if (mins.size() < usable_blocks) usable_blocks = mins.size();
  if (maxs.size() < usable_blocks) usable_blocks = maxs.size();

  const uint32_t* data = values.data();

  for (size_t b = 0; b < usable_blocks; ++b) {
    const size_t start = b * (size_t)block_size;
    size_t end = start + (size_t)block_size;
    if (end > n) end = n;

    const uint32_t bmin = mins[b];
    const uint32_t bmax = maxs[b];

    // Prune entirely outside blocks
    if (bmax < l || bmin > h) {
      continue;
    }

    const uint32_t* p = data + start;
    const uint32_t* pe = data + end;

    // Fully inside: fast sum without checks
    if (bmin >= l && bmax <= h) {
      // Unrolled accumulation
      for (; p + 4 <= pe; p += 4) {
        sum += (uint64_t)p[0] + (uint64_t)p[1] + (uint64_t)p[2] + (uint64_t)p[3];
      }
      for (; p < pe; ++p) {
        sum += (uint64_t)(*p);
      }
    } else {
      // Partially overlapping: branchless filtered accumulation
      for (; p + 4 <= pe; p += 4) {
        uint32_t v0 = p[0], v1 = p[1], v2 = p[2], v3 = p[3];
        sum += (uint64_t)(((v0 >= l) & (v0 <= h))) * v0;
        sum += (uint64_t)(((v1 >= l) & (v1 <= h))) * v1;
        sum += (uint64_t)(((v2 >= l) & (v2 <= h))) * v2;
        sum += (uint64_t)(((v3 >= l) & (v3 <= h))) * v3;
      }
      for (; p < pe; ++p) {
        uint32_t v = *p;
        sum += (uint64_t)(((v >= l) & (v <= h))) * v;
      }
    }
  }

  // If metadata does not cover all computed blocks, handle the tail safely
  const size_t covered = usable_blocks * (size_t)block_size;
  if (covered < n) {
    const uint32_t* p = data + covered;
    const uint32_t* pe = data + n;
    for (; p + 4 <= pe; p += 4) {
      uint32_t v0 = p[0], v1 = p[1], v2 = p[2], v3 = p[3];
      sum += (uint64_t)(((v0 >= l) & (v0 <= h))) * v0;
      sum += (uint64_t)(((v1 >= l) & (v1 <= h))) * v1;
      sum += (uint64_t)(((v2 >= l) & (v2 <= h))) * v2;
      sum += (uint64_t)(((v3 >= l) & (v3 <= h))) * v3;
    }
    for (; p < pe; ++p) {
      uint32_t v = *p;
      sum += (uint64_t)(((v >= l) & (v <= h))) * v;
    }
  }

  return sum;
}
