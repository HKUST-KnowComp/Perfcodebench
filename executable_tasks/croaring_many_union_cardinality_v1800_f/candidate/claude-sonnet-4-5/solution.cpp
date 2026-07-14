#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() <= 1) {
    return 0;
  }
  
  const std::size_t num_shards = offsets.size() - 1;
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    
    roaring_bitmap_t* bitmap = roaring_bitmap_of_ptr(count, &values[begin]);
    bitmaps.push_back(bitmap);
  }
  
  if (bitmaps.empty()) {
    return 0;
  }
  
  roaring_bitmap_t* result = bitmaps[0];
  
  for (std::size_t i = 1; i < bitmaps.size(); ++i) {
    roaring_bitmap_or_inplace(result, bitmaps[i]);
  }
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  
  for (std::size_t i = 1; i < bitmaps.size(); ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  roaring_bitmap_free(result);
  
  return cardinality;
}