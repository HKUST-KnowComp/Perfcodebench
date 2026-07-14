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
  if (values.empty() || window <= 0 || window > (int)values.size()) return 0;

  uint64_t hash = 0;
  size_t n = values.size();
  std::vector<size_t> deque(n);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t head = 0;
    size_t tail = 0;

    for (size_t i = 0; i < n; ++i) {
      if (head < tail && deque[head] <= i - window) {
        head++;
      }
      while (head < tail && values[deque[tail - 1]] <= values[i]) {
        tail--;
      }
      deque[tail++] = i;

      if (i >= (size_t)window - 1) {
        hash = mix(hash, values[deque[head]]);
      }
    }
  }
  return hash;
}