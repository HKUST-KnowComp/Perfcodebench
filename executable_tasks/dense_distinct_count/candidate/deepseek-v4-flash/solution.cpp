#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<int> last_seen(key_space + 1, 0);
  int current_iter = 0;
  uint64_t final_count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    ++current_iter;
    uint64_t count = 0;
    for (uint32_t key : keys) {
      if (last_seen[key] != current_iter) {
        last_seen[key] = current_iter;
        ++count;
      }
    }
    final_count = count;
  }
  return final_count;
}