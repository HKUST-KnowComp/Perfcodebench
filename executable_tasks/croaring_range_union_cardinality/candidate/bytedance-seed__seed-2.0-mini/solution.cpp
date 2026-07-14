#include "interface.h"
#include "roaring.h"

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t* const bitmap = roaring_bitmap_create();
  if (!bitmap) {
    return 0;
  }
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint64_t start_64 = static_cast<uint64_t>(starts[i]);
    const uint64_t end_64 = start_64 + lengths[i];
    roaring_bitmap_add_range(bitmap, start_64, end_64);
  }
  const uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return cardinality;
}