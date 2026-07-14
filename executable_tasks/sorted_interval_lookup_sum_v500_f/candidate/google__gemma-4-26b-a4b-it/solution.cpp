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
      // the interval is defined by the start point and the next start point, 
      // or specifically the 'ends' array provided in the interface.
      // However, the baseline code used: idx = upper_bound(starts, probe) - 1.
      // This means it finds the interval where starts[idx] <= probe < starts[idx+1].
      // We must ensure the probe is actually within the bounds of the interval.
      
      if (starts[interval_idx] <= probe_val) {
        // The baseline logic specifically used the index from upper_bound(starts). 
        // To preserve exact correctness with the baseline:
        // The baseline finds the largest idx where starts[idx] <= probe.
        // It then uses payloads[idx]. It does NOT check ends[idx].
        // To match the baseline's behavior exactly:
        current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum += current_iter_sum;
  }

  // The baseline returns the sum of the LAST iteration if it were a loop, 
  // but actually the baseline code resets 'sum = 0' inside the loop and returns 'sum'.
  // Wait, looking at baseline: 
  // sum = 0; (inside iter loop)
  // sum += ... (inside probe loop)
  // return sum; (outside iter loop)
  // This means the baseline actually returns the sum of ONLY the last iteration.
  // Let's re-examine baseline carefully:
  /*
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probes.size(); ++i) {
      ... sum += ...
    }
  }
  return sum;
  */
  // Yes, it returns the sum of the last iteration. 
  // To be safe and match the exact behavior (even if it looks like a bug in the baseline), 
  // I will replicate that logic.

  // Re-implementing to match baseline's exact return value (last iteration sum):
  uint64_t last_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    last_sum = 0;
    size_t interval_idx = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t probe_val = probes[i];
      while (interval_idx + 1 < num_intervals && starts[interval_idx + 1] <= probe_val) {
        interval_idx++;
      }
      if (starts[interval_idx] <= probe_val) {
        last_sum += static_cast<uint64_t>(payloads[interval_idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return last_sum;
}