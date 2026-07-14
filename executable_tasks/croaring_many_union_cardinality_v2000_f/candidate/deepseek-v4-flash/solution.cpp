#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* result = roaring_bitmap_create();
  size_t num_shards = offsets.size() - 1;
  for (size_t shard = 0; shard < num_shards; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    size_t len = end - begin;
    if (len > 0) {
      roaring_bitmap_add_many(result, len, &values[begin]);
    }
  }
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  roaring_bitmap_free(result);
  return cardinality;
}