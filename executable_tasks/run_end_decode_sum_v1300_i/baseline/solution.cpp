#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  std::vector<uint32_t> decoded;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.clear();
    uint32_t prev = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
      const uint32_t len = run_ends[i] - prev;
      for (uint32_t j = 0; j < len; ++j) {
        decoded.push_back(values[i]);
      }
      prev = run_ends[i];
    }
    sum = 0;
    for (uint32_t value : decoded) {
      sum += static_cast<uint64_t>(value);
    }
  }
  return sum;
}
