#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  if (iters <= 0) return 0;
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const std::size_t out_count = n - w + 1;
  const uint32_t* data = values.data();

  // Monotonic deque implemented as a ring buffer of indices.
  std::vector<std::size_t> dq(n);

  std::size_t head = 0;
  std::size_t tail = 0;

  // Build first window.
  for (std::size_t i = 0; i < w; ++i) {
    const uint32_t v = data[i];
    while (tail > head && data[dq[tail - 1]] <= v) {
      --tail;
    }
    dq[tail++] = i;
  }

  uint64_t hash = 1469598103934665603ULL;
  hash = mix(hash, data[dq[head]]);

  for (std::size_t i = w; i < n; ++i) {
    const std::size_t expired = i - w;
    if (dq[head] == expired) {
      ++head;
    }

    const uint32_t v = data[i];
    while (tail > head && data[dq[tail - 1]] <= v) {
      --tail;
    }
    dq[tail++] = i;
    hash = mix(hash, data[dq[head]]);
  }

  // Reuse the precomputed maxima sequence for remaining iterations.
  for (int iter = 1; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    head = 0;
    tail = 0;
    for (std::size_t i = 0; i < w; ++i) {
      const uint32_t v = data[i];
      while (tail > head && data[dq[tail - 1]] <= v) {
        --tail;
      }
      dq[tail++] = i;
    }
    h = mix(h, data[dq[head]]);
    for (std::size_t i = w; i < n; ++i) {
      const std::size_t expired = i - w;
      if (dq[head] == expired) {
        ++head;
      }
      const uint32_t v = data[i];
      while (tail > head && data[dq[tail - 1]] <= v) {
        --tail;
      }
      dq[tail++] = i;
      h = mix(h, data[dq[head]]);
    }

    hash = h;
  }

  return hash;
}
