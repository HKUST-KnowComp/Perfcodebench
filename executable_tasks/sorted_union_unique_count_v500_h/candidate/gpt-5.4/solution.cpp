#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;

  const uint32_t* lbase = left.empty() ? nullptr : left.data();
  const uint32_t* rbase = right.empty() ? nullptr : right.data();
  const size_t lsize = left.size();
  const size_t rsize = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l = lbase;
    const uint32_t* r = rbase;
    const uint32_t* const lend = lbase + lsize;
    const uint32_t* const rend = rbase + rsize;

    uint64_t c = 0;
    bool have_last = false;
    uint32_t last = 0;

    while (l != lend && r != rend) {
      uint32_t lv = *l;
      uint32_t rv = *r;
      uint32_t v;

      if (lv < rv) {
        v = lv;
        do {
          ++l;
        } while (l != lend && *l == v);
      } else if (rv < lv) {
        v = rv;
        do {
          ++r;
        } while (r != rend && *r == v);
      } else {
        v = lv;
        do {
          ++l;
        } while (l != lend && *l == v);
        do {
          ++r;
        } while (r != rend && *r == v);
      }

      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (l != lend) {
      uint32_t v = *l;
      do {
        ++l;
      } while (l != lend && *l == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (r != rend) {
      uint32_t v = *r;
      do {
        ++r;
      } while (r != rend && *r == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    count = c;
  }

  return count;
}
