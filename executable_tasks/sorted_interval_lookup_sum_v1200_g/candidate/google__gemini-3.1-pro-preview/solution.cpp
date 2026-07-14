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
  // The baseline implementation ignores 'ends' and assumes intervals are defined by 'starts'.
  // Since both 'starts' and 'probes' are sorted, we can use a two-pointer approach.
  (void)ends;

  const size_t n = starts.size();
  const size_t m = probes.size();
  if (n == 0 || m == 0) return 0;

  const uint32_t* __restrict s_ptr = starts.data();
  const uint32_t* __restrict p_ptr = probes.data();
  const uint32_t* __restrict w_ptr = weights.data();
  const uint32_t* __restrict pay_ptr = payloads.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total_sum = 0;
    size_t probe_idx = 0;
    size_t interval_idx = 0;

    // Skip probes that are before the first interval start to avoid underflow
    // (though the baseline logic implies probes[i] >= starts[0])
    while (probe_idx < m && p_ptr[probe_idx] < s_ptr[0]) {
        probe_idx++;
    }

    while (probe_idx < m) {
        // Find the correct interval for the current probe.
        // Since probes are sorted, we only ever move interval_idx forward.
        while (interval_idx + 1 < n && s_ptr[interval_idx + 1] <= p_ptr[probe_idx]) {
            interval_idx++;
        }

        const uint32_t next_interval_start = (interval_idx + 1 < n) ? s_ptr[interval_idx + 1] : 0xFFFFFFFF;
        const uint64_t current_payload = pay_ptr[interval_idx];
        
        // Process all probes that fall into the current interval [starts[idx], starts[idx+1]).
        // This loop is highly amenable to vectorization.
        uint64_t weight_acc = 0;
        size_t batch_start = probe_idx;
        while (probe_idx < m && p_ptr[probe_idx] < next_interval_start) {
            weight_acc += w_ptr[probe_idx];
            probe_idx++;
        }
        
        total_sum += current_payload * weight_acc;
    }
  }

  return total_sum;
}