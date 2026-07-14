#include "interface.h"
#include "roaring/roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* r = roaring_bitmap_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end   = offsets[shard + 1];
    roaring_bitmap_add_many(r, end - begin, values.data() + begin);
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}