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
  if (values.empty() || window <= 0) return 0;
  
  const size_t n = values.size();
  const size_t num_windows = n - window + 1;
  std::vector<uint32_t> deque_buffer(n);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    int head = 0;
    int tail = 0;

    for (size_t i = 0; i < n; ++i) {
      while (tail > head && values[deque_buffer[tail - 1]] <= values[i]) {
        tail--;
      }
      deque_buffer[tail++] = static_cast<uint32_t>(i);

      if (deque_buffer[head] <= static_cast<uint32_t>(i - window)) {
        head++;
      }

      if (i >= static_cast<size_t>(window - 1)) {
        hash = mix(hash, values[deque_buffer[head]]);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}