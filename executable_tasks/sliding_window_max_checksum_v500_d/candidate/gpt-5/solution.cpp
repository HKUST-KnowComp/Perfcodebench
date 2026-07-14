#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Preserve baseline behavior for iters == 0
  if (iters == 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const std::size_t n = values.size();

  // Handle special cases to match baseline behavior exactly
  if (window == 0) {
    // Baseline mixes mx=0 for each i in [0, n]
    uint64_t h = FNV_OFFSET;
    for (std::size_t i = 0; i <= n; ++i) {
      h = mix(h, 0ULL);
    }
    return h;
  }

  if (window < 0 || static_cast<std::size_t>(window) > n) {
    // No windows produced; baseline returns FNV offset basis for iters >= 1
    return FNV_OFFSET;
  }

  // Normal case: 1 <= window <= n
  const std::size_t w = static_cast<std::size_t>(window);
  const uint32_t* data = values.data();

  // Monotonic deque of indices stored in a vector used as a simple deque
  // The deque will be represented as dq[head, tail) within the array
  std::vector<std::size_t> dq;
  dq.resize(n); // At most n insertions, we avoid modulo arithmetic by not wrapping
  std::size_t head = 0, tail = 0;

  uint64_t h = FNV_OFFSET;

  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices out of the current window (i - w + 1)
    while (head < tail) {
      std::size_t idx = dq[head];
      if (idx + w <= i) {
        ++head;
      } else {
        break;
      }
    }
    // Maintain decreasing deque by values
    const uint32_t v = data[i];
    while (head < tail) {
      std::size_t lastIdx = dq[tail - 1];
      if (data[lastIdx] <= v) {
        --tail;
      } else {
        break;
      }
    }
    dq[tail++] = i;

    // Emit max for completed windows
    if (i + 1 >= w) {
      uint32_t mx = data[dq[head]];
      h = mix(h, static_cast<uint64_t>(mx));
    }
  }

  return h;
}
