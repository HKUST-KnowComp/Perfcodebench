#include "interface.h"
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Precompute selected indices
  const std::size_t n = keys.size();
  std::vector<uint32_t> selected;
  selected.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
  }

  uint64_t sum = 0;
  const std::size_t m = selected.size();
  const uint32_t* sel_ptr = selected.data();
  const uint32_t* pay_ptr = payloads.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    for (std::size_t j = 0; j < m; ++j) {
      s += static_cast<uint64_t>(pay_ptr[sel_ptr[j]]);
    }
    sum = s;
  }
  return sum;
}
