#include "interface.h"
#include <algorithm>

static inline uint64_t intersect_sum_merge(const uint32_t* a, size_t na, const uint32_t* b, size_t nb) {
  uint64_t sum = 0;
  const uint32_t* pa = a;
  const uint32_t* pb = b;
  const uint32_t* pa_end = a + na;
  const uint32_t* pb_end = b + nb;

  while (pa != pa_end && pb != pb_end) {
    uint32_t va = *pa;
    uint32_t vb = *pb;
    if (va < vb) {
      ++pa;
    } else if (vb < va) {
      ++pb;
    } else {
      sum += static_cast<uint64_t>(va);
      ++pa;
      ++pb;
    }
  }
  return sum;
}

static inline uint64_t intersect_sum_bs(const uint32_t* small, size_t ns, const uint32_t* large, size_t nl) {
  uint64_t sum = 0;
  const uint32_t* lbeg = large;
  const uint32_t* lend = large + nl;
  for (size_t i = 0; i < ns; ++i) {
    uint32_t v = small[i];
    const uint32_t* it = std::lower_bound(lbeg, lend, v);
    if (it != lend && *it == v) {
      sum += static_cast<uint64_t>(v);
    }
  }
  return sum;
}

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int /*iters*/) {
  const size_t na = lhs.size();
  const size_t nb = rhs.size();
  if (na == 0 || nb == 0) return 0;

  // Choose small and large for potential binary search strategy
  const uint32_t* a = lhs.data();
  const uint32_t* b = rhs.data();

  // Heuristic: if one array is significantly larger, binary search each element of the smaller in the larger.
  // Compare nb >= na * 32 to decide; otherwise use merge scan.
  if (na <= nb) {
    if (nb >= na * 32ull) {
      return intersect_sum_bs(a, na, b, nb);
    } else {
      return intersect_sum_merge(a, na, b, nb);
    }
  } else {
    if (na >= nb * 32ull) {
      return intersect_sum_bs(b, nb, a, na);
    } else {
      return intersect_sum_merge(a, na, b, nb);
    }
  }
}
