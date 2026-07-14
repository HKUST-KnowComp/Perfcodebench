#include "interface.h"

#include "roaring.h"

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  roaring_bulk_context_t context = {};
  for (uint32_t value : values) {
    roaring_bitmap_add_bulk(bitmap, &context, value);
  }
  const uint64_t total = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return total;
}
