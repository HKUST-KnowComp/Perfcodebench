#include "interface.h"

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const size_t N = values.size();
  uint64_t hash = 0;

  // Monotonic deque storage (indices), implemented over a vector to avoid extra includes
  std::vector<size_t> deq;
  deq.resize(N);
  const uint32_t* vals = N ? &values[0] : nullptr;

  const int w = window;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV-1a offset basis

    if (w <= 0 || static_cast<size_t>(w) > N) {
      continue; // No windows to process, identical to baseline behavior
    }

    size_t head = 0, tail = 0; // deque is [head, tail)

    for (size_t r = 0; r < N; ++r) {
      // Maintain decreasing deque by values
      while (tail > head) {
        size_t idx = deq[tail - 1];
        if (vals[idx] <= vals[r]) {
          --tail;
        } else {
          break;
        }
      }
      deq[tail++] = r;

      if (r + 1 >= static_cast<size_t>(w)) {
        size_t i = r + 1 - static_cast<size_t>(w);
        while (head < tail && deq[head] < i) {
          ++head; // pop front if out of window
        }
        uint32_t mx = vals[deq[head]];
        hash = mix(hash, static_cast<uint64_t>(mx));
      }
    }
  }

  return hash;
}
