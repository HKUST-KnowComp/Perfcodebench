#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const uint32_t *pb = boundaries.data();
  const uint32_t *pe = pb + boundaries.size();
  const uint32_t *pp = probes.data();
  const uint32_t *pep = pp + probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t *b = pb;
    uint64_t bucket = 0;
    for (const uint32_t *p = pp; p != pep; ++p) {
      while (b != pe && *b <= *p) { ++b; ++bucket; }
      sum += bucket;
    }
  }
  return sum;
}