#include "interface.h"
#include "roaring.h"

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t *lhs_bitmap = roaring_bitmap_create();
  roaring_bitmap_t *rhs_bitmap = roaring_bitmap_create();

  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    uint64_t start = lhs_starts[i];
    uint64_t end = start + lhs_lengths[i];
    if (end > start) {
      roaring_bitmap_add_range(lhs_bitmap, start, end);
    }
  }

  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    uint64_t start = rhs_starts[i];
    uint64_t end = start + rhs_lengths[i];
    if (end > start) {
      roaring_bitmap_add_range(rhs_bitmap, start, end);
    }
  }

  uint64_t cardinality = roaring_bitmap_and_cardinality(lhs_bitmap, rhs_bitmap);

  roaring_bitmap_free(lhs_bitmap);
  roaring_bitmap_free(rhs_bitmap);

  return cardinality;
}