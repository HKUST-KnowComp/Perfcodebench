#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* union_bitmap = values.empty() 
    ? roaring_bitmap_create() 
    : roaring_bitmap_create_with_capacity(values.size());
  
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const std::size_t count = end - begin;
    if (count == 0) continue;
    roaring_bitmap_add_many(union_bitmap, count, &values[begin]);
  }
  
  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);
  roaring_bitmap_free(union_bitmap);
  return cardinality;
}