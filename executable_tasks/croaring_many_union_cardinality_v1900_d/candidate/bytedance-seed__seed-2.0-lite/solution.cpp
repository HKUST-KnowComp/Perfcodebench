#include "interface.h"
#include <roaring/roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }
  
  roaring_bitmap_t* global_union = roaring_bitmap_create();
  
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const size_t count = end - begin;
    if (count == 0) continue;
    
    roaring_bitmap_add_many(global_union, count, values.data() + begin);
  }
  
  const uint64_t cardinality = roaring_bitmap_get_cardinality(global_union);
  roaring_bitmap_free(global_union);
  
  return cardinality;
}