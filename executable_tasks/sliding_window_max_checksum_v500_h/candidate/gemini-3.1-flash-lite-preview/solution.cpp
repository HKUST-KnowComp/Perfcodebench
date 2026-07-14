#include "interface.h"
#include <deque>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0 || window > static_cast<int>(values.size())) return 0;

  uint64_t final_hash = 0;
  const size_t n = values.size();
  std::vector<int> deque(n);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    int head = 0, tail = 0;

    for (size_t i = 0; i < n; ++i) {
      while (tail > head && values[deque[tail - 1]] <= values[i]) {
        tail--;
      }
      deque[tail++] = static_cast<int>(i);

      if (deque[head] <= static_cast<int>(i - window)) {
        head++;
      }

      if (static_cast<int>(i) >= window - 1) {
        hash = mix(hash, values[deque[head]]);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}