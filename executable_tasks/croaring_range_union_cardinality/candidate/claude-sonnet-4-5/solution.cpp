#include "interface.h"

#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint32_t begin = starts[i];
    const uint32_t length = lengths[i];
    if (length > 0) {
      const uint32_t end = begin + length - 1;
      roaring_bitmap_add_range_closed(bitmap, begin, end);
    }
  }
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  
  return cardinality;
}