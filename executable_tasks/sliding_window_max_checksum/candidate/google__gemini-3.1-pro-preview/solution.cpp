#include "interface.h"
#include <vector>
#include <cstdint>
#include <memory>

namespace {

// FNV-1a style mix function provided in the baseline
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

/**
 * Optimized sliding window maximum using a monotonic deque.
 * Complexity: O(N) per iteration, where N is values.size().
 * The deque stores indices of elements in the current window such that 
 * values[deque] is always non-increasing.
 */
uint64_t compute_sliding_window_max_hash(const std::vector<uint32_t>& values, int window, uint32_t* deque_buffer) {
  const size_t n = values.size();
  if (n == 0 || window <= 0) return 1469598103934665603ULL;

  uint64_t hash = 1469598103934665603ULL;
  int head = 0;
  int tail = 0;

  // Process the first window
  for (int i = 0; i < window; ++i) {
    uint32_t val = values[i];
    while (tail > head && values[deque_buffer[tail - 1]] <= val) {
      --tail;
    }
    deque_buffer[tail++] = i;
  }

  // The maximum for the first window
  hash = mix(hash, values[deque_buffer[head]]);

  // Process the rest of the windows
  for (size_t i = window; i < n; ++i) {
    // Remove indices that are out of the window boundary
    if (deque_buffer[head] <= static_cast<int>(i - window)) {
      ++head;
    }

    uint32_t val = values[i];
    // Maintain monotonic property: values[deque] must be descending
    while (tail > head && values[deque_buffer[tail - 1]] <= val) {
      --tail;
    }
    deque_buffer[tail++] = static_cast<uint32_t>(i);

    // The maximum of the current window is at the front of the deque
    hash = mix(hash, values[deque_buffer[head]]);
  }

  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0 || static_cast<size_t>(window) > values.size()) {
    return 0;
  }

  // Pre-allocate a buffer for the monotonic deque to avoid allocations inside the loop.
  // The deque will store at most 'window' indices, but 'values.size()' is a safe upper bound.
  std::unique_ptr<uint32_t[]> deque_buffer(new uint32_t[values.size()]);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = compute_sliding_window_max_hash(values, window, deque_buffer.get());
  }

  return hash;
}
