#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  std::size_t n = values.size();
  const uint32_t* data = n ? values.data() : nullptr;

  // Buffer for monotonic deque indices, allocated once per call
  std::vector<std::size_t> dq;
  dq.resize(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV offset basis

    // Match baseline semantics for edge cases
    if (window == 0) {
      // There are n + 1 zero-length windows; each mixes value 0
      std::size_t count = n + 1;
      for (std::size_t k = 0; k < count; ++k) {
        hash = mix(hash, 0ULL);
      }
      continue;
    }
    if (window < 0) {
      // No windows processed; hash remains the offset basis (as in baseline)
      continue;
    }

    std::size_t w = static_cast<std::size_t>(window);
    if (w == 0 || w > n) {
      // w==0 handled above; w>n: no windows to process
      continue;
    }

    // Monotonic decreasing deque of indices (store indices of candidates for max)
    std::size_t head = 0, tail = 0;

    for (std::size_t r = 0; r < n; ++r) {
      // Pop from tail while current value is greater or equal, to keep decreasing order
      uint32_t vr = data[r];
      while (tail > head && vr >= data[dq[tail - 1]]) {
        --tail;
      }
      dq[tail++] = r;

      if (r + 1 >= w) {
        std::size_t left = r + 1 - w; // left index of current window
        while (tail > head && dq[head] < left) {
          ++head;
        }
        uint32_t mx = data[dq[head]];
        hash = mix(hash, static_cast<uint64_t>(mx));
      }
    }
  }

  return hash;
}
