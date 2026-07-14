#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (key_space == 0) return 0;
  std::vector<uint32_t> version(key_space, 0);
  uint32_t cur_version = 0;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    ++cur_version;
    count = 0;
    for (uint32_t key : keys) {
      if (version[key] != cur_version) {
        version[key] = cur_version;
        ++count;
      }
    }
  }
  return count;
}