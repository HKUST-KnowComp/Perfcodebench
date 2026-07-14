#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;

  const uint32_t* a = left.data();
  const uint32_t* b = right.data();
  const size_t n = left.size();
  const size_t m = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    size_t j = 0;
    uint64_t c = 0;
    uint32_t last = 0;
    bool have_last = false;

    while (i < n && j < m) {
      uint32_t va = a[i];
      uint32_t vb = b[j];
      uint32_t v;

      if (va < vb) {
        v = va;
        do {
          ++i;
        } while (i < n && a[i] == v);
      } else if (vb < va) {
        v = vb;
        do {
          ++j;
        } while (j < m && b[j] == v);
      } else {
        v = va;
        do {
          ++i;
        } while (i < n && a[i] == v);
        do {
          ++j;
        } while (j < m && b[j] == v);
      }

      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (i < n) {
      uint32_t v = a[i];
      do {
        ++i;
      } while (i < n && a[i] == v);
      if (!have_last || v != last) {
        last = v;
        have_last = true;
        ++c;
      }
    }

    while (j < m) {
      uint32_t v = b[j];
      do {
        ++j;
      } while (j < m && b[j] == v);
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
