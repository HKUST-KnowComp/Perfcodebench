#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // The problem states that starts and probes are sorted.
  // We can use a two-pointer approach to achieve O(N + P) complexity per iteration.
  // Since the baseline uses std::upper_bound(starts.begin(), starts.end(), probes[i]) - 1,
  // it finds the largest index 'idx' such that starts[idx] <= probes[i].

  (void)ends;
  uint64_t total_sum = 0;

  const size_t num_starts = starts.size();
  const size_t num_probes = probes.size();

  if (num_starts == 0 || num_probes == 0) {
    return 0;
  }

  // Cache pointers to raw data to avoid vector overhead in the hot loop
  const uint32_t* __restrict p_starts = starts.data();
  const uint32_t* __restrict p_payloads = payloads.data();
  const uint32_t* __restrict p_probes = probes.data();
  const uint32_t* __restrict p_weights = weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = p_probes[i];
      
      // Advance idx to the largest index such that starts[idx] <= probe.
      // Since probes are sorted, idx only moves forward.
      // We check idx + 1 to see if the next interval start is still <= current probe.
      while (idx + 1 < num_starts && p_starts[idx + 1] <= probe) {
        idx++;
      }

      // The baseline logic: idx = upper_bound - 1.
      // If probe < starts[0], the baseline would result in idx = -1 (underflow).
      // We assume the input guarantees probe >= starts[0] based on the baseline's behavior,
      // but we handle the logic safely by starting idx at 0.
      current_sum += static_cast<uint64_t>(p_payloads[idx]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}
