#include "interface.h"
#include <roaring.h>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    uint32_t begin = lhs_starts[i];
    uint32_t end = begin + lhs_lengths[i];
    if (begin < end) {
      roaring_bitmap_add_range(lhs, begin, end);
    }
  }

  roaring_bitmap_t* rhs = roaring_bitmap_create();
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    uint32_t begin = rhs_starts[i];
    uint32_t end = begin + rhs_lengths[i];
    if (begin < end) {
      roaring_bitmap_add_range(rhs, begin, end);
    }
  }

  uint64_t total = roaring_bitmap_and_cardinality(lhs, rhs);

  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);

  return total;
}
