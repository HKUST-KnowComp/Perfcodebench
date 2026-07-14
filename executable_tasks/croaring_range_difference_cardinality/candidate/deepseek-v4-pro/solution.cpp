#include "interface.h"
#include "roaring.h"

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    uint64_t start = lhs_starts[i];
    uint64_t len   = lhs_lengths[i];
    uint64_t end   = start + len;
    roaring_bitmap_add_range(lhs, start, end);
  }

  roaring_bitmap_t* rhs = roaring_bitmap_create();
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    uint64_t start = rhs_starts[i];
    uint64_t len   = rhs_lengths[i];
    uint64_t end   = start + len;
    roaring_bitmap_add_range(rhs, start, end);
  }

  uint64_t card = roaring_bitmap_andnot_cardinality(lhs, rhs);

  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);

  return card;
}