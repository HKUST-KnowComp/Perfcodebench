#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, do nothing and return 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  uint64_t sum0 = 0;
  uint64_t sum1 = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    sum0 += static_cast<uint64_t>(v[i + 0]) * static_cast<uint64_t>(l[i + 0]);
    sum1 += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
    sum0 += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
    sum1 += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
    sum0 += static_cast<uint64_t>(v[i + 4]) * static_cast<uint64_t>(l[i + 4]);
    sum1 += static_cast<uint64_t>(v[i + 5]) * static_cast<uint64_t>(l[i + 5]);
    sum0 += static_cast<uint64_t>(v[i + 6]) * static_cast<uint64_t>(l[i + 6]);
    sum1 += static_cast<uint64_t>(v[i + 7]) * static_cast<uint64_t>(l[i + 7]);
  }

  uint64_t sum = sum0 + sum1;
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
  }

  return sum;
}
