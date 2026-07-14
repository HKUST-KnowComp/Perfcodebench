#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int /*iters*/) {
  const std::size_t n = values.size();
  if (n == 0) return 0;

  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  std::size_t i = 0;
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  // Unroll by 4 for better ILP and reduced loop overhead
  for (; i + 3 < n; i += 4) {
    s0 += static_cast<uint64_t>(v[i + 0]) * static_cast<uint64_t>(l[i + 0]);
    s1 += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
    s2 += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
    s3 += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  // Handle remaining tail elements
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
  }

  return sum;
}
