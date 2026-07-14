#include "interface.h"

#include <roaring.h>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();
  
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    if (end > begin) {
      roaring_bitmap_add_range_closed(lhs, begin, end - 1);
    }
  }
  
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    if (end > begin) {
      roaring_bitmap_add_range_closed(rhs, begin, end - 1);
    }
  }
  
  uint64_t result = roaring_bitmap_and_cardinality(lhs, rhs);
  
  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);
  
  return result;
}