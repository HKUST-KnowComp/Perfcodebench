#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;

  if (iters <= 0) return 0;
  if (window <= 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) hash = kOffset;
    return hash;
  }

  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  if (w > n) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) hash = kOffset;
    return hash;
  }

  uint64_t hash = 0;

  // Monotonic deque of indices stored in a ring buffer.
  std::vector<std::size_t> dq(n);

  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffset;
    std::size_t head = 0;
    std::size_t tail = 0;

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t v = values[i];

      while (head != tail) {
        std::size_t last_pos = (tail == 0 ? n - 1 : tail - 1);
        if (values[dq[last_pos]] > v) break;
        tail = last_pos;
      }

      dq[tail] = i;
      tail = (tail + 1 == n ? 0 : tail + 1);

      const std::size_t min_index = (i >= w ? i - w + 1 : 0);
      while (head != tail && dq[head] < min_index) {
        head = (head + 1 == n ? 0 : head + 1);
      }

      if (i + 1 >= w) {
        hash = mix(hash, values[dq[head]]);
      }
    }
  }

  return hash;
}
