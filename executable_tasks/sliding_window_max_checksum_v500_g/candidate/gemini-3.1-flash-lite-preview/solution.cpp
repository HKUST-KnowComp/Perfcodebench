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
  std::vector<uint32_t> q(values.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    int head = 0, tail = 0;

    for (int i = 0; i < (int)values.size(); ++i) {
      while (tail > head && values[q[tail - 1]] <= values[i]) {
        tail--;
      }
      q[tail++] = i;

      if (q[head] <= i - window) {
        head++;
      }

      if (i >= window - 1) {
        hash = mix(hash, values[q[head]]);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}