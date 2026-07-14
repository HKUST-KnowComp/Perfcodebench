#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
      current_sum += static_cast<uint64_t>(values[i]) * lengths[i];
    }
    final_sum = current_sum;
  }
  return final_sum;
}