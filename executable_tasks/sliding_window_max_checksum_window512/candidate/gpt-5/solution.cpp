#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline behavior for zero iterations: return initial hash (0)
  if (iters == 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  const int w = window;

  // FNV-1a offset basis
  uint64_t hash = 1469598103934665603ULL;

  // If window is non-positive or larger than the array, there are no windows to hash.
  if (w <= 0 || static_cast<std::size_t>(w) > n) {
    return hash;
  }

  const uint32_t* data = values.data();

  // Monotonic deque implemented via a vector as an index buffer
  // Stores indices of candidates for maximum in decreasing order of values
  std::vector<std::size_t> dq;
  dq.resize(n);
  std::size_t head = 0; // points to current front
  std::size_t tail = 0; // points one past current back

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t v = data[i];
    // Maintain decreasing order: pop smaller/equal elements from the back
    while (tail > head && v >= data[dq[tail - 1]]) {
      --tail;
    }
    dq[tail++] = i;

    // Once we've filled the first window, start producing maxima
    if (i + 1 >= static_cast<std::size_t>(w)) {
      const std::size_t start = i + 1 - static_cast<std::size_t>(w);
      // Evict elements that are out of the current window
      while (head < tail && dq[head] < start) {
        ++head;
      }
      // Front of deque is the index of the maximum in the current window
      const uint32_t mx = data[dq[head]];
      hash = mix(hash, static_cast<uint64_t>(mx));
    }
  }

  return hash;
}
