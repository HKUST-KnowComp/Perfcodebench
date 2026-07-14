#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  if (offsets.size() > 1) {
    roaring_bitmap_add_many(r, offsets.back(), values.data());
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}