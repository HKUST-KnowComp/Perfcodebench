#include "interface.h"
#include <roaring.h>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  // Initialize CRoaring bitmaps for both collections
  roaring_bitmap_t* lhs_roar = roaring_bitmap_create();
  roaring_bitmap_t* rhs_roar = roaring_bitmap_create();

  // Add all ranges from lhs, using CRoaring's efficient native range add
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    roaring_bitmap_add_range(lhs_roar, begin, end);
  }

  // Add all ranges from rhs
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    roaring_bitmap_add_range(rhs_roar, begin, end);
  }

  // Compute XOR (symmetric difference) cardinality directly via optimized CRoaring function
  const uint64_t result = roaring_bitmap_xor_cardinality(lhs_roar, rhs_roar);

  // Clean up allocated resources
  roaring_bitmap_free(lhs_roar);
  roaring_bitmap_free(rhs_roar);

  return result;
}