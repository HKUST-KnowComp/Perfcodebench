#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l = left.data();
    const uint32_t* r = right.data();
    const uint32_t* const lend = l + left.size();
    const uint32_t* const rend = r + right.size();

    uint64_t c = 0;

    while (l != lend && r != rend) {
      const uint32_t lv = *l;
      const uint32_t rv = *r;

      if (lv < rv) {
        ++c;
        do {
          ++l;
        } while (l != lend && *l == lv);
      } else if (rv < lv) {
        ++c;
        do {
          ++r;
        } while (r != rend && *r == rv);
      } else {
        ++c;
        do {
          ++l;
        } while (l != lend && *l == lv);
        do {
          ++r;
        } while (r != rend && *r == rv);
      }
    }

    while (l != lend) {
      const uint32_t v = *l;
      ++c;
      do {
        ++l;
      } while (l != lend && *l == v);
    }

    while (r != rend) {
      const uint32_t v = *r;
      ++c;
      do {
        ++r;
      } while (r != rend && *r == v);
    }

    count = c;
  }

  return count;
}
