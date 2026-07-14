#include "interface.h"

#include <vector>

static inline uint64_t compute_sum_once(const uint16_t* data, std::size_t n,
                                        uint32_t base, uint32_t low, uint32_t high) {
  // Assumes low <= high (checked by caller). Preserve 32-bit wrap-around semantics.
  uint64_t sum = 0;
  const uint32_t range = high - low; // valid since low <= high

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);

  // Unrolled by 8 for better throughput
  for (; i < n8; i += 8) {
    uint32_t v0 = base + static_cast<uint32_t>(data[i + 0]);
    uint32_t v1 = base + static_cast<uint32_t>(data[i + 1]);
    uint32_t v2 = base + static_cast<uint32_t>(data[i + 2]);
    uint32_t v3 = base + static_cast<uint32_t>(data[i + 3]);
    uint32_t v4 = base + static_cast<uint32_t>(data[i + 4]);
    uint32_t v5 = base + static_cast<uint32_t>(data[i + 5]);
    uint32_t v6 = base + static_cast<uint32_t>(data[i + 6]);
    uint32_t v7 = base + static_cast<uint32_t>(data[i + 7]);

    if (static_cast<uint32_t>(v0 - low) <= range) sum += static_cast<uint64_t>(v0);
    if (static_cast<uint32_t>(v1 - low) <= range) sum += static_cast<uint64_t>(v1);
    if (static_cast<uint32_t>(v2 - low) <= range) sum += static_cast<uint64_t>(v2);
    if (static_cast<uint32_t>(v3 - low) <= range) sum += static_cast<uint64_t>(v3);
    if (static_cast<uint32_t>(v4 - low) <= range) sum += static_cast<uint64_t>(v4);
    if (static_cast<uint32_t>(v5 - low) <= range) sum += static_cast<uint64_t>(v5);
    if (static_cast<uint32_t>(v6 - low) <= range) sum += static_cast<uint64_t>(v6);
    if (static_cast<uint32_t>(v7 - low) <= range) sum += static_cast<uint64_t>(v7);
  }

  for (; i < n; ++i) {
    uint32_t v = base + static_cast<uint32_t>(data[i]);
    if (static_cast<uint32_t>(v - low) <= range) {
      sum += static_cast<uint64_t>(v);
    }
  }

  return sum;
}

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Match baseline behavior: if iters == 0, loop body doesn't execute and sum remains 0.
  if (iters <= 0) return 0;

  // If range is empty (low > high), no values can match; baseline would sum to 0.
  if (low > high) return 0;

  // Compute the sum once; baseline returns the sum from a single iteration.
  const uint64_t sum_once = compute_sum_once(deltas.data(), deltas.size(), base, low, high);

  // Returning sum of the last (identical) iteration matches baseline semantics.
  return sum_once;
}
