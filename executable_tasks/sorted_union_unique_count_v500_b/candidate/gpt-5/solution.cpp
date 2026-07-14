#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* a = left.data();
    const uint32_t* b = right.data();
    const uint32_t* ae = a + left.size();
    const uint32_t* be = b + right.size();

    uint64_t count = 0;

    while (a < ae && b < be) {
      uint32_t va = *a;
      uint32_t vb = *b;
      if (va == vb) {
        // Count this value once and skip duplicates in both arrays
        ++count;
        do { ++a; } while (a < ae && *a == va);
        do { ++b; } while (b < be && *b == vb);
      } else if (va < vb) {
        // Count the run in left
        ++count;
        do { ++a; } while (a < ae && *a == va);
      } else { // va > vb
        // Count the run in right
        ++count;
        do { ++b; } while (b < be && *b == vb);
      }
    }

    // Process any remaining runs in left
    while (a < ae) {
      uint32_t v = *a;
      ++count;
      do { ++a; } while (a < ae && *a == v);
    }

    // Process any remaining runs in right
    while (b < be) {
      uint32_t v = *b;
      ++count;
      do { ++b; } while (b < be && *b == v);
    }

    result = count;
  }
  return result;
}
