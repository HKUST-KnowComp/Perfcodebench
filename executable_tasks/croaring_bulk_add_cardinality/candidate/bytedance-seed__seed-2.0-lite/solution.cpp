#include "interface.h"
#include <roaring.h>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  const size_t num_values = values.size();
  roaring_bitmap_add_many(bitmap, num_values, values.data());
  const uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return cardinality;
}