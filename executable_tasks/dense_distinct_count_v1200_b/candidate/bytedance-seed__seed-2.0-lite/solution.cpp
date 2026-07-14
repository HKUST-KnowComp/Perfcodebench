#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<uint32_t> seen(key_space, 0);
  uint64_t count = 0;
  for (int iter = 1; iter <= iters; ++iter) {
    uint64_t current_count = 0;
    for (uint32_t key : keys) {
      if (seen[key] != iter) {
        current_count++;
        seen[key] = iter;
      }
    }
    count = current_count;
  }
  return count;
}