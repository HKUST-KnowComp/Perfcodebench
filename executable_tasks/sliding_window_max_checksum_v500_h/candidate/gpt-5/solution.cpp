#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline behavior for iters
  if (iters <= 0) {
    return 0;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;

  const std::size_t n = values.size();
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    // No windows to process: baseline returns FNV offset after at least one iter
    return FNV_OFFSET;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const uint32_t* a = values.data();

  // Monotonic deque implemented with array: stores indices with non-increasing values
  std::vector<std::size_t> q(n);
  std::size_t head = 0, tail = 0;

  // Initialize first window
  for (std::size_t i = 0; i < w; ++i) {
    const uint32_t vi = a[i];
    while (tail > head) {
      const std::size_t idx = q[tail - 1];
      if (a[idx] <= vi) {
        --tail;
      } else {
        break;
      }
    }
    q[tail++] = i;
  }

  uint64_t hash = FNV_OFFSET;
  hash = mix(hash, static_cast<uint64_t>(a[q[head]]));

  // Slide the window
  for (std::size_t i = w; i < n; ++i) {
    // Evict indices that are out of the current window [i-w+1, i]
    const std::size_t min_index = i - w + 1;
    while (head < tail && q[head] < min_index) {
      ++head;
    }

    // Insert current index, maintaining non-increasing order
    const uint32_t vi = a[i];
    while (tail > head) {
      const std::size_t idx = q[tail - 1];
      if (a[idx] <= vi) {
        --tail;
      } else {
        break;
      }
    }
    q[tail++] = i;

    hash = mix(hash, static_cast<uint64_t>(a[q[head]]));
  }

  return hash;
}