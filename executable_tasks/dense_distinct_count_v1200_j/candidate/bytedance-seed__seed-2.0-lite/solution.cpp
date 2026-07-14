#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (keys.empty() || key_space == 0) return 0;
  
  std::vector<uint8_t> seen(key_space, 0);
  std::vector<uint32_t> touched;
  touched.reserve(keys.size());
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    touched.clear();
    uint64_t current_count = 0;
    for (uint32_t key : keys) {
      if (!seen[key]) {
        seen[key] = 1;
        current_count++;
        touched.push_back(key);
      }
    }
    count = current_count;
    // Reset only keys we modified to avoid costly full array reset
    for (uint32_t key : touched) {
      seen[key] = 0;
    }
  }
  return count;
}