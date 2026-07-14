#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t count_distinct_sorted_union_once(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right) {
  const uint32_t* l = left.data();
  const uint32_t* r = right.data();
  const uint32_t* const lend = l + left.size();
  const uint32_t* const rend = r + right.size();

  uint64_t count = 0;
  uint32_t last = 0;
  bool have_last = false;

  while (l != lend && r != rend) {
    uint32_t v;
    if (*l < *r) {
      v = *l;
      do {
        ++l;
      } while (l != lend && *l == v);
    } else if (*r < *l) {
      v = *r;
      do {
        ++r;
      } while (r != rend && *r == v);
    } else {
      v = *l;
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
      ++count;
    }
  }

  while (l != lend) {
    const uint32_t v = *l;
    do {
      ++l;
    } while (l != lend && *l == v);
    if (!have_last || v != last) {
      last = v;
      have_last = true;
      ++count;
    }
  }

  while (r != rend) {
    const uint32_t v = *r;
    do {
      ++r;
    } while (r != rend && *r == v);
    if (!have_last || v != last) {
      last = v;
      have_last = true;
      ++count;
    }
  }

  return count;
}

}  // namespace

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = count_distinct_sorted_union_once(left, right);
  }
  return count;
}
