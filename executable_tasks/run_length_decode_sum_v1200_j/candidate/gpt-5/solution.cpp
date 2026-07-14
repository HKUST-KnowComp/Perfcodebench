#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // Baseline semantics: if iters <= 0, the loop body never executes and sum remains 0
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();
  const std::size_t n = values.size();

  uint64_t acc0 = 0;
  uint64_t acc1 = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & static_cast<std::size_t>(~3u);
  for (; i < n4; i += 4) {
    acc0 += static_cast<uint64_t>(v[i    ]) * static_cast<uint64_t>(l[i    ]);
    acc1 += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
    acc0 += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
    acc1 += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
  }
  for (; i < n; ++i) {
    acc0 += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
  }

  uint64_t sum = acc0 + acc1;

  // As in the baseline, the same decoded stream is processed each iteration and
  // sum is reset every time; returning the per-iteration sum preserves semantics.
  return sum;
}
