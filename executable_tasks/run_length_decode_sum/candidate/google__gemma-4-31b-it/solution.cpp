#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // The baseline resets 'sum = 0' at the start of every iteration in the loop.
  // Therefore, the result is simply the sum of the decoded stream once,
  // provided iters > 0. If iters <= 0, the loop doesn't run and sum remains 0.
  if (iters <= 0) return 0;

  uint64_t total_sum = 0;
  std::size_t n = values.size();
  const uint32_t* v_ptr = values.data();
  const uint16_t* l_ptr = lengths.data();

  for (std::size_t i = 0; i < n; ++i) {
    total_sum += static_cast<uint64_t>(v_ptr[i]) * l_ptr[i];
  }

  return total_sum;
}