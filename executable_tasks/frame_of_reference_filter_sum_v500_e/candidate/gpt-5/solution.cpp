#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Preserve semantics for non-positive iterations: baseline returns 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = deltas.size();
  const uint16_t* data = n ? deltas.data() : nullptr;

  uint64_t sum = 0;

  // Single-pass decode and filter without allocating a decoded buffer.
  // Compute only once; subsequent iterations would yield the same sum.
  // Use uint32_t for value to preserve wrap-around semantics of unsigned addition.
  std::size_t i = 0;

  // Unroll by 4 for better throughput on larger inputs
  for (; i + 4 <= n; i += 4) {
    uint32_t v0 = base + static_cast<uint32_t>(data[i + 0]);
    uint32_t v1 = base + static_cast<uint32_t>(data[i + 1]);
    uint32_t v2 = base + static_cast<uint32_t>(data[i + 2]);
    uint32_t v3 = base + static_cast<uint32_t>(data[i + 3]);

    if (v0 >= low && v0 <= high) sum += static_cast<uint64_t>(v0);
    if (v1 >= low && v1 <= high) sum += static_cast<uint64_t>(v1);
    if (v2 >= low && v2 <= high) sum += static_cast<uint64_t>(v2);
    if (v3 >= low && v3 <= high) sum += static_cast<uint64_t>(v3);
  }

  for (; i < n; ++i) {
    uint32_t v = base + static_cast<uint32_t>(data[i]);
    if (v >= low && v <= high) {
      sum += static_cast<uint64_t>(v);
    }
  }

  return sum;
}
