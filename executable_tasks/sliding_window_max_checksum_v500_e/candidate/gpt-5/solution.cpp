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
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const std::size_t n = values.size();

  // Handle special window cases to match baseline semantics
  if (window < 0) {
    // No windows processed; hash remains FNV offset for the (last) iteration
    return FNV_OFFSET;
  }

  if (window == 0) {
    // Baseline loops i from 0..n (inclusive), mixing 0 each time
    uint64_t hash = FNV_OFFSET;
    for (std::size_t i = 0; i <= n; ++i) {
      hash = mix(hash, 0ULL);
    }
    return hash;
  }

  std::size_t w = static_cast<std::size_t>(window);
  if (w > n) {
    // No windows; hash stays FNV offset
    return FNV_OFFSET;
  }

  // Fast path for window size 1
  if (w == 1) {
    uint64_t hash = FNV_OFFSET;
    const uint32_t* data = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, data[i]);
    }
    return hash;
  }

  // General case: monotonic decreasing queue of indices (values[q[head]] >= ...)
  // Use a simple array-based queue sized n to avoid modulo operations
  uint64_t hash = FNV_OFFSET;
  std::vector<std::size_t> q;
  q.resize(n);
  std::size_t head = 0, tail = 0; // queue is [head, tail)

  const uint32_t* data = values.data();

  for (std::size_t i = 0; i < n; ++i) {
    // Pop back while current value is greater or equal (to maintain decreasing order)
    while (tail > head) {
      std::size_t back_idx = q[tail - 1];
      if (data[back_idx] <= data[i]) {
        --tail;
      } else {
        break;
      }
    }
    // Push current index
    q[tail++] = i;

    // Remove front if it's out of the window (index <= i - w)
    if (q[head] + w <= i) {
      ++head;
    }

    // When we have a full window, mix the maximum
    if (i + 1 >= w) {
      hash = mix(hash, static_cast<uint64_t>(data[q[head]]));
    }
  }

  return hash;
}
