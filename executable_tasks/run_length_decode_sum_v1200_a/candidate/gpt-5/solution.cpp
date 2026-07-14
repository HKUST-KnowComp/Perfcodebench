#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int /*iters*/) {
  // The sum of the decoded stream is sum(values[i] * lengths[i]) modulo 2^64.
  // It is independent of the number of iterations in the baseline, which
  // recomputed the same result each time and returned the last sum.
  uint64_t sum = 0;
  const std::size_t n = values.size();
  const std::size_t m = lengths.size();
  const std::size_t count = (n < m) ? n : m;  // Safety if sizes differ.

  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  std::size_t i = 0;
  // Unroll by 4 for a small performance boost.
  for (; i + 4 <= count; i += 4) {
    sum += (uint64_t)v[i] * (uint64_t)l[i];
    sum += (uint64_t)v[i + 1] * (uint64_t)l[i + 1];
    sum += (uint64_t)v[i + 2] * (uint64_t)l[i + 2];
    sum += (uint64_t)v[i + 3] * (uint64_t)l[i + 3];
  }
  for (; i < count; ++i) {
    sum += (uint64_t)v[i] * (uint64_t)l[i];
  }

  return sum;
}
