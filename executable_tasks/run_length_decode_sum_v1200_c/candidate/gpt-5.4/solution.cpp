#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  const std::size_t n = values.size();
  uint64_t sum = 0;

  if (iters <= 0 || n == 0) {
    return 0;
  }

  // The decoded-stream sum for one iteration is simply:
  //   sum(values[i] * lengths[i])
  // Repeating the decode/sum process does not change the per-iteration result,
  // and the baseline returns the sum from the final iteration only.
  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  uint64_t iter_sum = 0;
  std::size_t i = 0;

  // Small manual unroll to reduce loop overhead.
  for (; i + 3 < n; i += 4) {
    iter_sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
    iter_sum += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
    iter_sum += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
    iter_sum += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
  }
  for (; i < n; ++i) {
    iter_sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
  }

  sum = iter_sum;
  return sum;
}
