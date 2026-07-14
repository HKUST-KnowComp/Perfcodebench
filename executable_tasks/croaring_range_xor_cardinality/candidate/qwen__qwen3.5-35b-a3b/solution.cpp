#include "interface.h"
#include <roaring.h>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring::RoaringBitmap lhs_bitmap;
  roaring::RoaringBitmap rhs_bitmap;

  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    lhs_bitmap.add_range(lhs_starts[i], lhs_starts[i] + lhs_lengths[i]);
  }

  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    rhs_bitmap.add_range(rhs_starts[i], rhs_starts[i] + rhs_lengths[i]);
  }

  roaring::RoaringBitmap xor_bitmap = lhs_bitmap.xor(rhs_bitmap);
  return xor_bitmap.get_cardinality();
}