#include "interface.h"
#include <roaring.h>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  
  const size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
  
  if (num_shards == 0) {
    return 0;
  }
  
  // Create bitmaps for each shard
  std::vector<roaring_bitmap_t*> bitmaps(num_shards);
  
  for (size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    
    // Create bitmap from sorted array (values within each shard are sorted)
    bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
  }
  
  // Compute union of all bitmaps using optimized heap-based algorithm
  roaring_bitmap_t* result = roaring_bitmap_or_many_heap(num_shards, const_cast<const roaring_bitmap_t**>(bitmaps.data()));
  
  // Get cardinality
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  
  // Cleanup
  roaring_bitmap_free(result);
  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  
  return cardinality;
}