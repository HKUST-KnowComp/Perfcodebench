#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
  const size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
  for (size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    if (count == 0) continue;
    const uint32_t* shard_values = values.data() + begin;
    roaring_bitmap_add_many(union_bitmap, count, shard_values);
  }
  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);
  roaring_bitmap_free(union_bitmap);
  return cardinality;
}