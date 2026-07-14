#include "interface.h"
#include <vector>
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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

  // Pre-allocate a buffer for the monotonic queue to avoid repeated allocations
  // The queue stores indices of elements in 'values'
  std::vector<size_t> deque(n);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t head = 0;
    size_t tail = 0;

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

      // Once we have reached the first full window, start hashing the max
      if (i >= w - 1) {
        hash = mix(hash, values[deque[head]]);
      }
    }
    final_hash = hash; // The problem asks for the checksum of the process; 
                       // based on the baseline, it resets hash per iteration and returns the last one.
                       // Wait, the baseline does: hash = 1469... inside the loop, then returns the final hash.
                       // Let's re-verify: the baseline resets 'hash' every iteration and returns the result of the last iteration.
  }
  
  // To match the baseline's behavior exactly: the baseline resets 'hash' inside the 'iters' loop.
  // The final return is the 'hash' from the last iteration.
  return final_hash;
}