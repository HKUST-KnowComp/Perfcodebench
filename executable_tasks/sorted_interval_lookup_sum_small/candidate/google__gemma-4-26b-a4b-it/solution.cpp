#include "interface.h"

#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (probes.empty() || starts.empty()) return 0;

  uint64_t total_sum = 0;
  const size_t n_intervals = starts.size();
  const size_t n_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t interval_idx = 0;

    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = probes[i];

      // Advance interval_idx to the largest index such that starts[interval_idx] <= probe
      // Since probes are sorted, we don't need to reset interval_idx
      while (interval_idx + 1 < n_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }

      // Check if the current probe falls within the interval [starts[idx], ends[idx]]
      // Note: The baseline logic used upper_bound(starts) - 1, which finds the largest start <= probe.
      // We must verify if that specific interval actually contains the probe via the 'ends' array.
      if (starts[interval_idx] <= probe && probe <= ends[interval_idx]) {
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration if it were a single loop, 
  // but the baseline code actually accumulates 'sum = 0' inside the loop and returns the final 'sum'.
  // Wait, looking at baseline: 'sum = 0;' is inside 'for (int iter = 0; iter < iters; ++iter)'.
  // This means it returns the sum of the LAST iteration only. 
  // Let's re-examine baseline: 
  // sum = 0; (inside iter loop)
  // sum += ... (inside probe loop)
  // return sum; (outside iter loop)
  // This means the baseline returns the sum of the very last iteration.

  // Re-implementing to match baseline behavior exactly:
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_sum = 0;
    size_t interval_idx = 0;
    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = probes[i];
      while (interval_idx + 1 < n_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }
      if (starts[interval_idx] <= probe && probe <= ends[interval_idx]) {
        final_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return final_sum;
}