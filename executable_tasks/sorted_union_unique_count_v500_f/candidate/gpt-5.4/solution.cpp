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

    while (i < ln && j < rn) {
      uint32_t lv = l[i];
      uint32_t rv = r[j];

      if (lv < rv) {
        ++c;
        do {
          ++i;
        } while (i < ln && l[i] == lv);
      } else if (rv < lv) {
        ++c;
        do {
          ++j;
        } while (j < rn && r[j] == rv);
      } else {
        ++c;
        do {
          ++i;
        } while (i < ln && l[i] == lv);
        do {
          ++j;
        } while (j < rn && r[j] == rv);
      }
    }

    while (i < ln) {
      uint32_t lv = l[i];
      ++c;
      do {
        ++i;
      } while (i < ln && l[i] == lv);
    }

    while (j < rn) {
      uint32_t rv = r[j];
      ++c;
      do {
        ++j;
      } while (j < rn && r[j] == rv);
    }

    count = c;
  }

  return count;
}
