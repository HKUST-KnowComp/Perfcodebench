#include "interface.h"

#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l = left.data();
    const uint32_t* r = right.data();
    const uint32_t* le = l + left.size();
    const uint32_t* re = r + right.size();

    uint64_t count = 0;

    while (l < le && r < re) {
      uint32_t va = *l;
      uint32_t vb = *r;
      if (va < vb) {
        ++count;
        do { ++l; } while (l < le && *l == va);
      } else if (vb < va) {
        ++count;
        do { ++r; } while (r < re && *r == vb);
      } else { // va == vb
        ++count;
        do { ++l; } while (l < le && *l == va);
        do { ++r; } while (r < re && *r == vb);
      }
    }

    while (l < le) {
      ++count;
      uint32_t v = *l;
      do { ++l; } while (l < le && *l == v);
    }

    while (r < re) {
      ++count;
      uint32_t v = *r;
      do { ++r; } while (r < re && *r == v);
    }

    result = count;
  }
  return result;
}
