#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {

  const std::size_t n = keys.size();
  if (n == 0 || iters <= 0) return 0;

  const uint32_t* __restrict kptr = keys.data();
  const uint8_t*  __restrict fptr = flags.data();
  const uint32_t* __restrict pptr = payloads.data();

  // Build selection vector once (inputs are constant across iters)
  std::vector<uint32_t> selected;
  selected.reserve(n < 4096 ? n : 4096);

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = kptr[i];
    if (__builtin_expect(fptr[i] && k < threshold && ((k & 7u) == 0u), 0)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
  }

  const std::size_t sel_count = selected.size();
  if (sel_count == 0) return 0;

  const uint32_t* __restrict sel_ptr = selected.data();

  uint64_t sum = 0;

  // Iterate gather-sum phase
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t j = 0;

    // Unroll by 4 for small selection counts
    const std::size_t unroll_end = sel_count & ~std::size_t(3);
    for (; j < unroll_end; j += 4) {
      local_sum += pptr[sel_ptr[j]];
      local_sum += pptr[sel_ptr[j + 1]];
      local_sum += pptr[sel_ptr[j + 2]];
      local_sum += pptr[sel_ptr[j + 3]];
    }
    for (; j < sel_count; ++j) {
      local_sum += pptr[sel_ptr[j]];
    }
    sum = local_sum;
  }

  return sum;
}
