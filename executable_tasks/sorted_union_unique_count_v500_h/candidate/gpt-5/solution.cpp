#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
  const uint32_t* a = left.data();
  const uint32_t* b = right.data();
  const uint32_t* ae = a + left.size();
  const uint32_t* be = b + right.size();

  uint64_t count = 0;

  // Merge-like traversal, skipping duplicate runs within each array
  while (a < ae && b < be) {
    uint32_t va = *a;
    uint32_t vb = *b;

    if (va < vb) {
      // Count this distinct value from left and skip all its duplicates
      ++count;
      do { ++a; } while (a < ae && *a == va);
    } else if (vb < va) {
      // Count this distinct value from right and skip all its duplicates
      ++count;
      do { ++b; } while (b < be && *b == vb);
    } else {
      // Same value in both arrays: count once and skip duplicates in both
      ++count;
      uint32_t v = va; // same as vb
      do { ++a; } while (a < ae && *a == v);
      do { ++b; } while (b < be && *b == v);
    }
  }

  // Process remaining distinct runs in the tail of left
  while (a < ae) {
    uint32_t v = *a;
    ++count;
    do { ++a; } while (a < ae && *a == v);
  }

  // Process remaining distinct runs in the tail of right
  while (b < be) {
    uint32_t v = *b;
    ++count;
    do { ++b; } while (b < be && *b == v);
  }

  return count;
}
