#include "interface.h"

#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t num_shards = offsets.size() - 1;
  
  if (num_shards == 0) {
    return 0;
  }
  
  if (num_shards == 1) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    if (end > begin) {
      roaring_bitmap_add_many(bitmap, end - begin, &values[begin]);
    }
    uint64_t result = roaring_bitmap_get_cardinality(bitmap);
    roaring_bitmap_free(bitmap);
    return result;
  }
  
  // Create bitmaps for all shards
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    if (end > begin) {
      roaring_bitmap_add_many(bitmap, end - begin, &values[begin]);
    }
    bitmaps.push_back(bitmap);
  }
  
  // Compute union cardinality
  roaring_bitmap_t* result_bitmap = bitmaps[0];
  
  for (std::size_t i = 1; i < num_shards; ++i) {
    roaring_bitmap_or_inplace(result_bitmap, bitmaps[i]);
  }
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(result_bitmap);
  
  // Free all bitmaps
  for (std::size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  
  return cardinality;
}