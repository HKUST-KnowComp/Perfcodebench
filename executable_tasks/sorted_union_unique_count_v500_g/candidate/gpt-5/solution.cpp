#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;

  const uint32_t* lbase = left.data();
  const uint32_t* rbase = right.data();
  const size_t ln = left.size();
  const size_t rn = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l = lbase;
    const uint32_t* r = rbase;
    const uint32_t* le = lbase + ln;
    const uint32_t* re = rbase + rn;

    uint64_t c = 0;

    while (l < le && r < re) {
      uint32_t a = *l;
      uint32_t b = *r;
      if (a < b) {
        ++c;
        do { ++l; } while (l < le && *l == a);
      } else if (b < a) {
        ++c;
        do { ++r; } while (r < re && *r == b);
      } else { // a == b
        ++c;
        uint32_t v = a;
        do { ++l; } while (l < le && *l == v);
        do { ++r; } while (r < re && *r == v);
      }
    }

    while (l < le) {
      ++c;
      uint32_t v = *l;
      do { ++l; } while (l < le && *l == v);
    }

    while (r < re) {
      ++c;
      uint32_t v = *r;
      do { ++r; } while (r < re && *r == v);
    }

    count = c;
  }

  return count;
}
