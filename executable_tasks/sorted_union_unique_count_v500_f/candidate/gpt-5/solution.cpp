#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t result = 0;
  const size_t n = left.size();
  const size_t m = right.size();
  const uint32_t* lp = n ? left.data() : nullptr;
  const uint32_t* rp = m ? right.data() : nullptr;

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t count = 0;

    while (i < n && j < m) {
      uint32_t a = lp[i];
      uint32_t b = rp[j];
      if (a == b) {
        ++count;
        do { ++i; } while (i < n && lp[i] == a);
        do { ++j; } while (j < m && rp[j] == b);
      } else if (a < b) {
        ++count;
        do { ++i; } while (i < n && lp[i] == a);
      } else { // b < a
        ++count;
        do { ++j; } while (j < m && rp[j] == b);
      }
    }

    while (i < n) {
      uint32_t a = lp[i];
      ++count;
      do { ++i; } while (i < n && lp[i] == a);
    }
    while (j < m) {
      uint32_t b = rp[j];
      ++count;
      do { ++j; } while (j < m && rp[j] == b);
    }

    result = count;
  }

  return result;
}
