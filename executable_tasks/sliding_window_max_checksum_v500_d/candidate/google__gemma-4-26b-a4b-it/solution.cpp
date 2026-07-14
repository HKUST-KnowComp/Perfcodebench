#include "interface.h"
#include <vector>
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0 || static_cast<size_t>(window) > values.size()) {
    return 0;
  }

  size_t n = values.size();
  size_t w = static_cast<size_t>(window);
  size_t num_windows = n - w + 1;
  
  // Pre-calculate the maxima for one pass to avoid re-calculating in the 'iters' loop
  // Using a monotonic deque approach (O(N))
  std::vector<uint32_t> maxima(num_windows);
  std::vector<size_t> deque(n);
  size_t head = 0;
  size_t tail = 0;

  for (size_t i = 0; i < n; ++i) {
    // Remove indices out of window
    if (tail > head && deque[head] <= i - w) {
      head++;
    }
    // Remove elements smaller than current from back
    while (tail > head && values[deque[tail - 1]] <= values[i]) {
      tail--;
    }
    deque[tail++] = i;

    // If window is full, record max
    if (i >= w - 1) {
      maxima[i - w + 1] = values[deque[head]];
    }
  }

  uint64_t total_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_windows; ++i) {
      total_hash = mix(total_hash, maxima[i]);
    }
  }

  return total_hash;
}