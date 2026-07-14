#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* result = roaring_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;
    
    roaring_bitmap_t* shard_bitmap = roaring_create();
    roaring_add_many(shard_bitmap, &values[begin], end - begin);
    roaring_or_inplace(result, shard_bitmap);
    roaring_free(shard_bitmap);
  }
  uint64_t cardinality = roaring_get_cardinality(result);
  roaring_free(result);
  return cardinality;
}