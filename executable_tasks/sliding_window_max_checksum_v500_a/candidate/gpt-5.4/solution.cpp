#include "interface.h"

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
  if (iters <= 0) return 0;

  uint64_t hash = 0;

  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const std::size_t out_count = n - w + 1;

  std::vector<std::size_t> deq(n);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    std::size_t head = 0;
    std::size_t tail = 0;

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t v = values[i];

      while (head < tail && values[deq[tail - 1]] <= v) {
        --tail;
      }
      deq[tail++] = i;

      const std::size_t min_idx = (i >= w) ? (i - w + 1) : 0;
      while (head < tail && deq[head] < min_idx) {
        ++head;
      }

      if (i + 1 >= w) {
        hash = mix(hash, values[deq[head]]);
      }
    }

    (void)out_count;
  }

  return hash;
}
