#include "interface.h"
#include <roaring.h>
#include <cstdlib>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (num_shards == 0) {
    return 0;
  }

  // Build a roaring bitmap for each shard
  roaring_bitmap_t** bitmaps = (roaring_bitmap_t**)malloc(num_shards * sizeof(roaring_bitmap_t*));
  
  for (size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t count = end - begin;
    
    if (count > 0) {
      // Data is sorted, use optimized construction
      bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
    } else {
      bitmaps[shard] = roaring_bitmap_create();
    }
  }

  // Compute union of all bitmaps using heap-based many-way union
  roaring_bitmap_t* result = roaring_bitmap_or_many_heap(num_shards, (const roaring_bitmap_t**)bitmaps);
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);

  // Cleanup
  roaring_bitmap_free(result);
  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  free(bitmaps);

  return cardinality;
}