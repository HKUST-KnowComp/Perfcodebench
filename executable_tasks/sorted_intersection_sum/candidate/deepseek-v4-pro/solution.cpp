#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  const uint32_t* l = lhs.data();
  const uint32_t* r = rhs.data();
  const size_t nl = lhs.size();
  const size_t nr = rhs.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t s = 0;
    while (i < nl && j < nr) {
      uint32_t lv = l[i];
      uint32_t rv = r[j];
      if (lv < rv) {
        ++i;
      } else if (rv < lv) {
        ++j;
      } else {
        s += static_cast<uint64_t>(lv);
        ++i;
        ++j;
      }
    }
    sum = s;
  }
  return sum;
}
