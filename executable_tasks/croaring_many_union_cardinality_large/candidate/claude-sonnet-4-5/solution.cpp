#include "interface.h"

#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t num_shards = offsets.size() - 1;
  if (num_shards == 0) {
    return 0;
  }
  
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    if (count > 0) {
      roaring_bitmap_add_many(bitmap, count, &values[begin]);
    }
    bitmaps.push_back(bitmap);
  }
  
  uint64_t cardinality = 0;
  if (num_shards == 1) {
    cardinality = roaring_bitmap_get_cardinality(bitmaps[0]);
  } else {
    roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards, 
                                                       const_cast<const roaring_bitmap_t**>(bitmaps.data()));
    cardinality = roaring_bitmap_get_cardinality(result);
    roaring_bitmap_free(result);
  }
  
  for (roaring_bitmap_t* bitmap : bitmaps) {
    roaring_bitmap_free(bitmap);
  }
  
  return cardinality;
}