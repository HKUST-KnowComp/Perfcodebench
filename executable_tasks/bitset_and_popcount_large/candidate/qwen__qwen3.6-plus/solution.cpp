#include "interface.h"
#include <cstdint>
#include <vector>

#if defined(_MSC_VER)
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
  const std::size_t n = left.size();
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();
  
  for (std::size_t i = 0; i < n; ++i) {
    count += POPCOUNT64(l[i] & r[i]);
  }
  return count;
}