#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;

  const uint32_t* l = left.data();
  const uint32_t* r = right.data();
  const size_t ln = left.size();
  const size_t rn = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    size_t j = 0;
    uint64_t c = 0;
    uint32_t last = 0;
    bool have_last = false;

    while (i < ln && j < rn) {
      uint32_t v;
      const uint32_t lv = l[i];
      const uint32_t rv = r[j];

      if (lv < rv) {
        v = lv;
        do {
          ++i;
        } while (i < ln && l[i] == v);
      } else if (rv < lv) {
        v = rv;
        do {
          ++j;
        } while (j < rn && r[j] == v);
      } else {
        v = lv;
        do {
          ++i;
        } while (i < ln && l[i] == v);
        do {
          ++j;
        } while (j < rn && r[j] == v);
      }

      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (i < ln) {
      const uint32_t v = l[i];
      do {
        ++i;
      } while (i < ln && l[i] == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (j < rn) {
      const uint32_t v = r[j];
      do {
        ++j;
      } while (j < rn && r[j] == v);
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
