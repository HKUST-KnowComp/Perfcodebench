#include "interface.h"

#include <roaring.h>
#include <vector>
#include <cstddef>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  
  if (offsets.size() <= 1) {
    return 0;
  }
  
  const size_t num_shards = offsets.size() - 1;
  
  // Build roaring bitmaps for each shard
  std::vector<roaring_bitmap_t*> bitmaps(num_shards);
  
  for (size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    
    // Create bitmap from sorted array (values within each shard are sorted)
    bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
  }
  
  // Compute union using heap-based many-way OR
  roaring_bitmap_t* result = roaring_bitmap_or_many_heap(num_shards, const_cast<const roaring_bitmap_t**>(bitmaps.data()));
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  
  // Cleanup
  roaring_bitmap_free(result);
  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  
  return cardinality;
}