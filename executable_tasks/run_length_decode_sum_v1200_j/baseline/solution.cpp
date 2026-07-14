#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  std::vector<uint32_t> decoded;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.clear();
    for (std::size_t i = 0; i < values.size(); ++i) {
      for (uint16_t j = 0; j < lengths[i]; ++j) {
        decoded.push_back(values[i]);
      }
    }
    sum = 0;
    for (uint32_t value : decoded) {
      sum += static_cast<uint64_t>(value);
    }
  }
  return sum;
}
