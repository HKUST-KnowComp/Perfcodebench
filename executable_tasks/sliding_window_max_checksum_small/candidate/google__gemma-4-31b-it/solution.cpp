#include "interface.h"
#include <deque>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0) return 0;
  if (window > (int)values.size()) return 0;

  uint64_t final_hash = 0;
  const size_t n = values.size();
  const size_t w = static_cast<size_t>(window);

  // Pre-allocate buffer for the monotonic queue to avoid repeated allocations
  std::vector<size_t> deque(n);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    size_t head = 0;
    size_t tail = 0;

    for (size_t i = 0; i < n; ++i) {
      // Remove indices that are out of the current window
      if (head < tail && deque[head] <= i - w) {
        head++;
      }

      // Maintain monotonic property: remove elements smaller than current
      while (head < tail && values[deque[tail - 1]] <= values[i]) {
        tail--;
      }
      deque[tail++] = i;

      // Start hashing once the first window is complete
      if (i >= w - 1) {
        current_hash = mix(current_hash, values[deque[head]]);
      }
    }
    final_hash = current_hash;
  }

  return final_hash;
}