#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int /*iters*/) {
  const std::size_t n = deltas.size();
  if (n == 0) return 0;

  const uint32_t* p = deltas.data();
  uint64_t sum = static_cast<uint64_t>(base) * static_cast<uint64_t>(n);
  uint64_t w = static_cast<uint64_t>(n);

  std::size_t i = 0;
  // Unroll by 4 for better throughput
  for (; i + 4 <= n; i += 4) {
    sum += static_cast<uint64_t>(p[i    ]) * (w     );
    sum += static_cast<uint64_t>(p[i + 1]) * (w - 1u);
    sum += static_cast<uint64_t>(p[i + 2]) * (w - 2u);
    sum += static_cast<uint64_t>(p[i + 3]) * (w - 3u);
    w -= 4u;
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(p[i]) * w;
    --w;
  }

  return sum;
}
