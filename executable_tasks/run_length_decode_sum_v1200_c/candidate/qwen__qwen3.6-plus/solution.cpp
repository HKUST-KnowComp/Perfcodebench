#include "interface.h"

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  const size_t n = values.size();
  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current = 0;
    for (size_t i = 0; i < n; ++i) {
      current += static_cast<uint64_t>(v[i]) * l[i];
    }
    sum = current;
  }
  return sum;
}