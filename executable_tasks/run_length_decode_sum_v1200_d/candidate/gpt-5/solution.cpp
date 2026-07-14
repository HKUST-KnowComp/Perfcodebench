#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int /*iters*/) {
  // Compute the sum directly from run-length encoding without decoding.
  const std::size_t n = values.size();
  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;

  // Unrolled loop for better throughput.
  for (; i + 3 < n; i += 4) {
    s0 += (uint64_t)v[i] * (uint64_t)l[i];
    s1 += (uint64_t)v[i + 1] * (uint64_t)l[i + 1];
    s2 += (uint64_t)v[i + 2] * (uint64_t)l[i + 2];
    s3 += (uint64_t)v[i + 3] * (uint64_t)l[i + 3];
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  // Remainder.
  for (; i < n; ++i) {
    sum += (uint64_t)v[i] * (uint64_t)l[i];
  }

  return sum;
}
