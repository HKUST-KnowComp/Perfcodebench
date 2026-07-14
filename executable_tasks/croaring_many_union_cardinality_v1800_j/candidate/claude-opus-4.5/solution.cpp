#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  if (!offsets.empty() && offsets.size() > 1) {
    const uint32_t total = offsets.back();
    if (total > 0) {
      roaring_bitmap_add_many(r, total, values.data());
    }
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}