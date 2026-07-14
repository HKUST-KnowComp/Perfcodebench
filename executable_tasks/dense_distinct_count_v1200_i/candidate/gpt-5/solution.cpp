#include "interface.h"

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) {
    // No valid keys possible
    return 0;
  }

  // Epoch-stamped visitation array: stamps[k] == epoch means key k seen in current iteration
  std::vector<uint32_t> stamps(key_space, 0);
  uint32_t epoch = 1; // start from 1; 0 means "never seen"

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Handle epoch overflow: clear stamps and reset epoch to 1
    if (epoch == 0) {
      // manual clear to avoid extra includes
      for (uint32_t i = 0; i < key_space; ++i) stamps[i] = 0;
      epoch = 1;
    }

    uint64_t cnt = 0;
    uint32_t* s = stamps.data();

    // Assuming keys are within [0, key_space) as per bounded key space contract
    for (size_t i = 0; i < keys.size(); ++i) {
      uint32_t k = keys[i];
      // No bounds check for performance (keys are bounded by key_space)
      if (s[k] != epoch) {
        s[k] = epoch;
        ++cnt;
      }
    }

    count = cnt; // last iteration's result
    ++epoch; // advance epoch for next iteration; may wrap to 0 triggering clear next time
  }

  return count;
}
