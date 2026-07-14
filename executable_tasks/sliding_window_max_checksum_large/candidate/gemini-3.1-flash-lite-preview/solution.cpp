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

  uint64_t final_hash = 0;
  size_t n = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    std::deque<size_t> dq;

    for (size_t i = 0; i < n; ++i) {
      // Remove indices out of window
      if (!dq.empty() && dq.front() <= i - window) {
        dq.pop_front();
      }

      // Maintain monotonic decreasing property
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      dq.push_back(i);

      // Start hashing once the first window is full
      if (i >= (size_t)window - 1) {
        hash = mix(hash, values[dq.front()]);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}