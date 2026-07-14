#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  size_t num_runs = values.size();
  
  if (num_runs == 0) {
    return 0;
  }

  // Pre-calculate run lengths to avoid repeated subtraction in the loop
  // and to minimize work inside the 'iters' loop.
  std::vector<uint64_t> run_lengths(num_runs);
  uint32_t prev_end = 0;
  for (size_t i = 0; i < num_runs; ++i) {
    run_lengths[i] = static_cast<uint64_t>(run_ends[i] - prev_end);
    prev_end = run_ends[i];
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_runs; ++i) {
      current_iter_sum += run_lengths[i] * static_cast<uint64_t>(values[i]);
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}