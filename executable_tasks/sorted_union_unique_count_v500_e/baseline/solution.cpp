#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  std::vector<uint32_t> merged;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    merged = left;
    merged.insert(merged.end(), right.begin(), right.end());
    std::sort(merged.begin(), merged.end());
    count = static_cast<uint64_t>(std::unique(merged.begin(), merged.end()) - merged.begin());
  }
  return count;
}
