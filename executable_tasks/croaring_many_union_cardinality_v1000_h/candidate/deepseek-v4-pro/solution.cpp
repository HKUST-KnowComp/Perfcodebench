#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* result = roaring_bitmap_create();
  for (size_t i = 0; i + 1 < offsets.size(); ++i) {
    uint32_t begin = offsets[i];
    uint32_t end = offsets[i+1];
    if (begin >= end) continue;
    roaring_bitmap_t* shard = roaring_bitmap_of_ptr(end - begin, values.data() + begin);
    roaring_bitmap_or_inplace(result, shard);
    roaring_bitmap_free(shard);
  }
  uint64_t card = roaring_bitmap_get_cardinality(result);
  roaring_bitmap_free(result);
  return card;
}