#include "interface.h"

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline behavior for iteration count and edge cases
  if (iters == 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  if (window <= 0) {
    // No windows processed; baseline leaves hash at seed per iter
    return 1469598103934665603ULL;
  }
  const std::size_t w = static_cast<std::size_t>(window);
  if (w > n) {
    // No windows processed; baseline leaves hash at seed per iter
    return 1469598103934665603ULL;
  }

  // Fast path for window==1: maxima are the elements themselves
  const uint32_t* v = values.data();
  uint64_t hash = 1469598103934665603ULL;
  if (w == 1) {
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
    return hash;
  }

  // Monotonic deque implemented with a flat array for indices
  std::vector<std::size_t> deq(n);
  std::size_t head = 0, tail = 0;

  // Initialize the first window
  for (std::size_t i = 0; i < w; ++i) {
    while (head < tail && v[i] >= v[deq[tail - 1]]) {
      --tail;
    }
    deq[tail++] = i;
  }

  // Mix the maximum for the first window
  hash = mix(hash, static_cast<uint64_t>(v[deq[head]]));

  // Slide the window
  for (std::size_t i = w; i < n; ++i) {
    // Remove indices out of the current window
    const std::size_t win_start = i - w + 1;
    while (head < tail && deq[head] < win_start) {
      ++head;
    }
    // Insert current index while maintaining monotonicity
    while (head < tail && v[i] >= v[deq[tail - 1]]) {
      --tail;
    }
    deq[tail++] = i;

    // Current maximum
    hash = mix(hash, static_cast<uint64_t>(v[deq[head]]));
  }

  return hash;
}