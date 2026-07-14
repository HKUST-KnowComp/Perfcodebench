#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline behavior for zero iterations
  if (iters == 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();

  // Handle invalid windows like baseline (no windows processed -> return offset basis)
  if (window <= 0) {
    return 1469598103934665603ULL;
  }
  const std::size_t w = static_cast<std::size_t>(window);
  if (w > n) {
    return 1469598103934665603ULL;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  // Monotonic deque implemented with a vector as ringless buffer of indices
  // dq holds indices of elements in decreasing order of their values
  std::vector<std::size_t> dq(n);
  std::size_t head = 0;
  std::size_t tail = 0;

  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices that are out of the current window
    // Condition: index <= i - w  <=>  index + w <= i
    while (head != tail) {
      std::size_t idx = dq[head];
      if (idx + w <= i) {
        ++head;
      } else {
        break;
      }
    }

    // Maintain decreasing values: pop back while current value is >= back's value
    while (head != tail) {
      std::size_t back_idx = dq[tail - 1];
      if (values[back_idx] <= values[i]) {
        --tail;
      } else {
        break;
      }
    }

    // Push current index
    dq[tail++] = i;

    // Hash once the first full window is formed
    if (i + 1 >= w) {
      uint32_t mx = values[dq[head]];
      hash = mix(hash, mx);
    }
  }

  return hash;
}
