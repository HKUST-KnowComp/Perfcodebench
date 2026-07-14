#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  std::vector<uint32_t> selected;
  selected.reserve(n);

  const uint32_t* k = keys.data();
  const uint8_t*  f = flags.data();

  for (std::size_t i = 0; i < n; ++i) {
    if (f[i] && k[i] < threshold && (k[i] & 7u) == 0u) {
      selected.push_back(static_cast<uint32_t>(i));
    }
  }

  const std::size_t m = selected.size();
  const uint32_t* sel = selected.data();
  const uint32_t* pay = payloads.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t j = 0; j < m; ++j) {
      sum += static_cast<uint64_t>(pay[sel[j]]);
    }
  }

  return sum;
}