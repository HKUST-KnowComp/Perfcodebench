#include "interface.h"

#include "roaring.h"

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint64_t begin = lhs_starts[i];
    roaring_bitmap_add_range_closed(lhs, begin, begin + lhs_lengths[i] - 1);
  }
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint64_t begin = rhs_starts[i];
    roaring_bitmap_add_range_closed(rhs, begin, begin + rhs_lengths[i] - 1);
  }
  const uint64_t total = roaring_bitmap_and_cardinality(lhs, rhs);
  roaring_bitmap_free(rhs);
  roaring_bitmap_free(lhs);
  return total;
}
