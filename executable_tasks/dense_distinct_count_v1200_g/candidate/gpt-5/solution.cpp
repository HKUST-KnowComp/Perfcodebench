#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  const size_t n = keys.size();
  if (key_space == 0 || n == 0 || iters <= 0) {
    return 0;
  }

  std::vector<uint32_t> seen(key_space, 0);
  uint32_t epoch = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t unique = 0;
    const uint32_t current_epoch = epoch;

    const uint32_t* data = keys.data();
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = data[i];
      uint32_t &mark = seen[k];
      if (mark != current_epoch) {
        mark = current_epoch;
        ++unique;
        if (unique == key_space) {
          // All possible keys seen; early exit
          break;
        }
      }
    }

    count = unique;

    // Advance epoch, handle rare wrap-around by clearing
    ++epoch;
    if (epoch == 0) {
      for (size_t i = 0; i < seen.size(); ++i) seen[i] = 0;
      epoch = 1;
    }
  }

  return count;
}
