#include "interface.h"

#include <roaring.h>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();
  
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint64_t begin = lhs_starts[i];
    const uint64_t end = begin + lhs_lengths[i];
    roaring_bitmap_add_range(lhs, begin, end);
  }
  
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint64_t begin = rhs_starts[i];
    const uint64_t end = begin + rhs_lengths[i];
    roaring_bitmap_add_range(rhs, begin, end);
  }
  
  roaring_bitmap_andnot_inplace(lhs, rhs);
  uint64_t result = roaring_bitmap_get_cardinality(lhs);
  
  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);
  
  return result;
}