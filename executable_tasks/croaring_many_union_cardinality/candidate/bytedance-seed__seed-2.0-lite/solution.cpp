#include "interface.h"
#include <roaring.h>

uint64_t_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
  if (!union_bitmap) return 0;

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;
    
    const size_t count = end - begin;
    const uint32_t* shard_values = values.data() + begin;
    roaring_bitmap_add_many(union_bitmap, count, shard_values);
  }

  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);
  roaring_bitmap_free(union_bitmap);
  return cardinality;
}