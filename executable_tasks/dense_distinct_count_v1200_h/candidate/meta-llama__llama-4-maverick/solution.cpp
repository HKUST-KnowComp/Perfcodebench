#include "interface.h"

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<bool> seen(key_space, false);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint32_t key : keys) {
      if (!seen[key]) {
        seen[key] = true;
        count++;
      }
    }
    for (uint32_t key : keys) {
      seen[key] = false;
    }
  }
  return count;
}