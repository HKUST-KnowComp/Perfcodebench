#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  for (size_t s = 0; s + 1 < offsets.size(); ++s) {
    uint32_t begin = offsets[s];
    uint32_t end = offsets[s+1];
    if (begin < end) {
      roaring_bitmap_add_many(r, end - begin, &values[begin]);
    }
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}