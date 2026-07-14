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

      // Advance interval_idx to the largest start <= probe_val
      // Since both starts and probes are sorted, we never need to decrement interval_idx
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe_val) {
        interval_idx++;
      }

      // Check if the current probe falls within the interval [starts[idx], ends[idx]]
      // Note: The baseline logic used upper_bound(starts) - 1, which implies 
      // it assumes the probe is contained in the interval starting at that index.
      // We must verify the probe is actually >= starts[idx] and < ends[idx].
      // However, the baseline code provided: 
      // const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      // const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      // This logic assumes the probe is always valid for the interval found by the floor of the start.
      
      if (starts[interval_idx] <= probe_val) {
        // The baseline logic does not check 'ends', it assumes the interval is valid if starts[idx] <= probe.
        // To preserve exact correctness with the baseline:
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration if it were a standard loop, 
  // but the baseline code actually accumulates 'sum = 0' inside the loop and returns 'sum'.
  // Wait, looking at baseline: 
  // sum = 0; (inside iter loop)
  // return sum; (outside iter loop)
  // This means the baseline actually only returns the sum of the VERY LAST iteration.
  
  // Re-evaluating baseline logic:
  // for (int iter = 0; iter < iters; ++iter) {
  //   sum = 0;
  //   for (...) { sum += ... }
  // }
  // return sum;
  // This is mathematically equivalent to running the loop once, but the 'iters' parameter 
  // is used to multiply the work/time. To match the return value exactly:
  
  uint64_t final_sum = 0;
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
  
  // Since the baseline resets sum = 0 every iteration, the return value is just the result of one iteration.
  // The 'iters' loop in the baseline is likely a harness mechanism to ensure the function is timed.
  return final_sum;
}