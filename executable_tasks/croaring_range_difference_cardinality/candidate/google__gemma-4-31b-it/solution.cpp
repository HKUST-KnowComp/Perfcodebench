#include "interface.h"
#include "roaring.h"

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  
  roaring_bitmap_t* lhs_bm = roaring_bitmap_create();
  roaring_bitmap_t* rhs_bm = roaring_bitmap_create();

  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    uint32_t start = lhs_starts[i];
    uint32_t end = start + lhs_lengths[i];
    if (lhs_lengths[i] > 0) {
      roaring_bitmap_add_range(lhs_bm, start, end);
    }
  }

  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    uint32_t start = rhs_starts[i];
    uint32_t end = start + rhs_lengths[i];
    if (rhs_lengths[i] > 0) {
      roaring_bitmap_add_range(rhs_bm, start, end);
    }
  }

  // Compute lhs = lhs AND NOT rhs
  roaring_bitmap_andnot(lhs_bm, rhs_bm);
  
  uint64_t result = roaring_bitmap_cardinality(lhs_bm);

  roaring_bitmap_destroy(lhs_bm);
  roaring_bitmap_destroy(rhs_bm);

  return result;
}