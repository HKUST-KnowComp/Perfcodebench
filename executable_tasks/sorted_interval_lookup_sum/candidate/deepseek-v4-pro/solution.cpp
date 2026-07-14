#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  (void)iters;

  const size_t n_probes = probes.size();
  if (n_probes == 0) return 0;
  const size_t m_starts = starts.size();

  const uint32_t* s_ptr = starts.data();
  const uint32_t* p_ptr = payloads.data();
  const uint32_t* w_ptr = weights.data();
  const uint32_t* pr_ptr = probes.data();

  uint64_t sum = 0;
  size_t intv_idx = 0;
  for (size_t i = 0; i < n_probes; ++i) {
    const uint32_t probe = pr_ptr[i];
    while (intv_idx + 1 < m_starts && s_ptr[intv_idx + 1] <= probe) {
      ++intv_idx;
    }
    sum += static_cast<uint64_t>(p_ptr[intv_idx]) * static_cast<uint64_t>(w_ptr[i]);
  }

  return sum;
}