#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);

  uint64_t hash = 0;

  if (iters <= 0) return hash;
  if (window <= 0 || w > n) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  const std::size_t out_count = n - w + 1;

  // Monotonic deque implemented with indices in a flat array for low overhead.
  std::vector<std::size_t> dq(n);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    std::size_t head = 0;
    std::size_t tail = 0;

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t v = values[i];

      while (head < tail && values[dq[tail - 1]] <= v) {
        --tail;
      }
      dq[tail++] = i;

      const std::size_t min_index = (i >= w) ? (i - w + 1) : 0;
      while (head < tail && dq[head] < min_index) {
        ++head;
      }

      if (i + 1 >= w) {
        hash = mix(hash, values[dq[head]]);
      }
    }

    (void)out_count;
  }

  return hash;
}
