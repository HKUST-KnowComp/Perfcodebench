#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  std::size_t n = values.size();
  if (lengths.size() < n) n = lengths.size();

  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    s0 += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
    s1 += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
    s2 += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
    s3 += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
  }
  uint64_t sum = s0 + s1 + s2 + s3;
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
  }

  return sum;
}
