#include "interface.h"

#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (probes.empty() || starts.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  const size_t num_probes = probes.size();
  const size_t num_intervals = starts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t interval_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe_val = probes[i];

      // Advance interval_idx to the largest index such that starts[interval_idx] <= probe_val
      // Since probes are sorted, we don't need to reset interval_idx
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe_val) {
        interval_idx++;
      }

      // Check if the current probe falls within the interval [starts[idx], ends[idx])
      // Note: The baseline logic used upper_bound(starts) - 1, which implies 
      // the interval is defined by the start point and we assume the probe is valid 
      // if it's >= starts[idx]. However, the baseline code didn't check 'ends'.
      // To preserve exact correctness with the baseline: 
      // the baseline logic was: idx = upper_bound(starts, probe) - 1.
      // This means it finds the largest idx where starts[idx] <= probe.
      
      if (starts[interval_idx] <= probe_val) {
        // The baseline code did not actually check if probe < ends[idx].
        // It simply took the payload of the interval that 'starts' before or at the probe.
        // To preserve the exact behavior of the baseline (as requested by 'preserving correctness'),
        // we follow the baseline's logic: find idx where starts[idx] is the largest value <= probe.
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration if it were to overwrite, 
  // but the loop structure 'sum = 0; ... sum += ...' inside 'iters' actually 
  // accumulates the sum of the last iteration only if we look at the return value.
  // Wait, looking at baseline: 
  // for (int iter = 0; iter < iters; ++iter) { sum = 0; for (...) { sum += ... } }
  // This means the function returns the sum of the LAST iteration only.
  
  // Let's re-read baseline carefully:
  // uint64_t sum = 0;
  // for (int iter = 0; iter < iters; ++iter) {
  //   sum = 0;
  //   for (size_t i = 0; i < probes.size(); ++i) {
  //     ... sum += ...
  //   }
  // }
  // return sum;
  // Yes, it returns the sum of the final iteration. 
  // My optimized version should return the sum of the final iteration to be 'correct' relative to baseline.

  // Re-implementing to match baseline's exact return behavior:
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_sum = 0;
    size_t interval_idx = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe_val = probes[i];
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe_val) {
        interval_idx++;
      }
      if (starts[interval_idx] <= probe_val) {
        final_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return final_sum;
}