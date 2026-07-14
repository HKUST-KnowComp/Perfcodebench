#include "interface.h"
#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint32_t begin = starts[i];
    const uint32_t end = begin + lengths[i];
    roaring_bitmap_add_range(union_bitmap, begin, end);
  }
  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);
  roaring_bitmap_free(union_bitmap);
  return cardinality;
}