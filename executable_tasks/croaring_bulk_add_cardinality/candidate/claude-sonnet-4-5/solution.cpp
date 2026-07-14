#include "interface.h"

#include <roaring.h>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  
  if (!values.empty()) {
    roaring_bitmap_add_many(bitmap, values.size(), values.data());
  }
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  
  return cardinality;
}