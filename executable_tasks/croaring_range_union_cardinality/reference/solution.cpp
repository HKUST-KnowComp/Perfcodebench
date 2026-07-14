#include "interface.h"

#include "roaring.h"

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint64_t begin = starts[i];
    const uint64_t end = begin + lengths[i];
    roaring_bitmap_add_range_closed(bitmap, begin, end - 1);
  }
  const uint64_t total = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return total;
}
