#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline semantics for iters <= 0
  if (iters <= 0) return 0ULL;

  const std::size_t n = values.size();

  // If no valid windows exist, baseline returns the FNV offset after the last iter
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    return 1469598103934665603ULL;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const uint32_t* v = values.data();

  uint64_t hash = 1469598103934665603ULL;

  // Fast path: window size 1 -> each element is the max of its window
  if (w == 1) {
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
    return hash;
  }

  // Monotonic deque implemented with an index array and head/tail pointers
  // Stores indices of elements in decreasing order of values
  std::vector<std::size_t> dq(n);
  std::size_t head = 0, tail = 0;

  // Initialize deque with the first window [0, w)
  for (std::size_t i = 0; i < w; ++i) {
    std::size_t t = tail;
    while (t > head) {
      std::size_t idx = dq[t - 1];
      if (v[idx] >= v[i]) break;
      --t;
    }
    tail = t;
    dq[tail++] = i;
  }

  // Mix max for the first window
  hash = mix(hash, static_cast<uint64_t>(v[dq[head]]));

  // Slide the window across the array
  for (std::size_t i = w; i < n; ++i) {
    // Remove indices that fall out of the current window
    if (dq[head] <= i - w) {
      ++head;
    }

    // Maintain decreasing order: remove from back while current value is larger
    std::size_t t = tail;
    while (t > head) {
      std::size_t idx = dq[t - 1];
      if (v[idx] >= v[i]) break;
      --t;
    }
    tail = t;
    dq[tail++] = i;

    // Current window maximum is at the front
    hash = mix(hash, static_cast<uint64_t>(v[dq[head]]));
  }

  return hash;
}
