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
  if (values.size() < static_cast<size_t>(window)) return 0;

  const size_t n = values.size();
  const size_t w = static_cast<size_t>(window);
  
  // Pre-allocate a buffer for the monotonic queue to avoid repeated allocations
  // The queue stores indices of elements in the current window
  std::vector<size_t> deque(n);

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    int head = 0;
    int tail = 0;

    for (size_t i = 0; i < n; ++i) {
      // Remove indices that are out of the current window
      if (head < tail && deque[head] <= i - w) {
        head++;
      }

      // Remove elements smaller than the current element from the back
      while (head < tail && values[deque[tail - 1]] <= values[i]) {
        tail--;
      }

      deque[tail++] = i;

      // The window is fully formed starting from index w - 1
      if (i >= w - 1) {
        hash = mix(hash, values[deque[head]]);
      }
    }
    final_hash = hash; // The problem asks for the checksum of the process
    // Note: The baseline resets hash inside the loop and returns the last iteration's result
    // based on the logic 'hash = 1469598103934665603ULL' inside the iter loop.
  }

  return final_hash;
}