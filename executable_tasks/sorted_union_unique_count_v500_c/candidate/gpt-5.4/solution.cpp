#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t result = 0;

  const uint32_t* lbase = left.empty() ? nullptr : left.data();
  const uint32_t* rbase = right.empty() ? nullptr : right.data();
  const size_t lsize = left.size();
  const size_t rsize = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    size_t j = 0;
    uint64_t count = 0;
    uint32_t last = 0;
    bool have_last = false;

    while (i < lsize && j < rsize) {
      uint32_t lv = lbase[i];
      uint32_t rv = rbase[j];
      uint32_t v;

      if (lv < rv) {
        v = lv;
        do {
          ++i;
        } while (i < lsize && lbase[i] == v);
      } else if (rv < lv) {
        v = rv;
        do {
          ++j;
        } while (j < rsize && rbase[j] == v);
      } else {
        v = lv;
        do {
          ++i;
        } while (i < lsize && lbase[i] == v);
        do {
          ++j;
        } while (j < rsize && rbase[j] == v);
      }

      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++count;
      }
    }

    while (i < lsize) {
      uint32_t v = lbase[i];
      do {
        ++i;
      } while (i < lsize && lbase[i] == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++count;
      }
    }

    while (j < rsize) {
      uint32_t v = rbase[j];
      do {
        ++j;
      } while (j < rsize && rbase[j] == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++count;
      }
    }

    result = count;
  }

  return result;
}
