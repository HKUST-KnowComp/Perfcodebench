#include "interface.h"
#include <deque>

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

  // Pre-allocate a buffer to act as a deque to avoid repeated allocations
  // A simple array-based deque is faster than std::deque
  std::vector<size_t> deque_buf(n);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    int head = 0;
    int tail = 0;

    for (size_t i = 0; i < n; ++i) {
      // Remove indices that are out of the current window
      if (head < tail && deque_buf[head] <= i - w) {
        head++;
      }

      // Remove elements smaller than the current element from the back
      while (head < tail && values[deque_buf[tail - 1]] <= values[i]) {
        tail--;
      }

      deque_buf[tail++] = i;

      // The first window is complete at index w - 1
      if (i >= w - 1) {
        hash = mix(hash, values[deque_buf[head]]);
      }
    }
    final_hash = hash;
  }

  return final_hash;
}