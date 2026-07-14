#include "interface.h"
#include <vector>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#define POPCOUNT64(x) __popcnt64(x)
#else
#define POPCOUNT64(x) __builtin_popcountll(x)
#endif

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  (void)iters;
  uint64_t count = 0;
  const size_t n = left.size();
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
  size_t i = 0;
  for (; i + 3 < n; i += 4) {
    c0 += POPCOUNT64(l[i] & r[i]);
    c1 += POPCOUNT64(l[i+1] & r[i+1]);
    c2 += POPCOUNT64(l[i+2] & r[i+2]);
    c3 += POPCOUNT64(l[i+3] & r[i+3]);
  }
  count = c0 + c1 + c2 + c3;
  for (; i < n; ++i) {
    count += POPCOUNT64(l[i] & r[i]);
  }

  return count;
}