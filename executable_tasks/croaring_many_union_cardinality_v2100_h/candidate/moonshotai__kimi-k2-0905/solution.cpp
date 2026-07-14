#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end   = offsets[shard + 1];
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(r, values[i]);
    }
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}