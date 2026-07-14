#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // If no iterations requested, return 0 (matches baseline behavior)
  if (iters <= 0) return 0ULL;

  const std::size_t n = values.size();
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;

  // If window is invalid or larger than input, no windows exist; hash remains offset basis
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    return FNV_OFFSET;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const uint32_t* a = n ? values.data() : nullptr;

  // Monotonic deque implemented with a vector as a ringless deque via head/tail indices
  // Stores indices of elements; values are in non-increasing order in deque
  std::vector<std::size_t> dq(n); // capacity sufficient for worst case
  std::size_t head = 0, tail = 0; // [head, tail)

  uint64_t hash = FNV_OFFSET;

  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices out of the current window [i-w+1, i]
    while (head < tail && dq[head] + w <= i) {
      ++head;
    }
    const uint32_t vi = a[i];
    // Maintain decreasing values in deque; remove smaller or equal from back
    while (head < tail && a[dq[tail - 1]] <= vi) {
      --tail;
    }
    dq[tail++] = i;

    // When the first full window is formed, mix the current maximum
    if (i + 1 >= w) {
      hash = mix(hash, static_cast<uint64_t>(a[dq[head]]));
    }
  }

  return hash;
}
