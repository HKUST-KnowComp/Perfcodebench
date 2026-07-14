#include "interface.h"
#include <vector>
#include <algorithm>

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
      uint32_t probe = probes[i];

      // Advance interval_idx to the largest index such that starts[interval_idx] <= probe
      // Since probes are sorted, we don't need to reset interval_idx
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe) {
        interval_idx++;
      }

      // Check if the current probe falls within the interval [starts[idx], ends[idx])
      // Note: The baseline logic used upper_bound(starts) - 1, which implies 
      // the interval is defined by the start point and the next start point (or end).
      // However, the baseline specifically used 'idx' from upper_bound, which maps 
      // a probe to the interval that started most recently.
      if (starts[interval_idx] <= probe) {
        // The baseline logic: idx = upper_bound(starts, probe) - 1
        // This effectively finds the interval where starts[idx] <= probe < starts[idx+1]
        // We must ensure the probe is also < ends[idx] if the problem implies strict interval bounds.
        // But looking at the baseline: it only checks starts[idx] <= probe via the index math.
        // To preserve exact correctness with the baseline, we follow its logic.
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration's sum if it were a standard loop,
  // but the baseline code actually accumulates 'sum = 0' inside the loop and returns 'sum'.
  // Wait, looking at baseline: 'sum = 0;' is inside the 'iter' loop. 
  // This means the baseline returns the sum of ONLY the last iteration.
  // Let's re-examine baseline: 
  // for (int iter = 0; iter < iters; ++iter) { sum = 0; ... sum += ... } 
  // return sum; 
  // This means the 'iters' loop is actually redundant for the final result, 
  // it just repeats the same calculation 'iters' times and returns the last one.
  
  // Re-implementing to match baseline's exact behavior (returning the sum of the last iteration):
  uint64_t final_sum = 0;
  size_t interval_idx = 0;
  for (size_t i = 0; i < num_probes; ++i) {
    uint32_t probe = probes[i];
    while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe) {
      interval_idx++;
    }
    if (starts[interval_idx] <= probe) {
      final_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
    }
  }
  
  return final_sum;
}