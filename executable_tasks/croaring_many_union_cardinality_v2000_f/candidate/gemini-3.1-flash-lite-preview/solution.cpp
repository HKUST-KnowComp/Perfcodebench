#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() <= 1) return 0;

  roaring_bitmap_t* result = roaring_bitmap_create();
  
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;

    // Create a temporary bitmap for the current shard
    roaring_bitmap_t* shard_bm = roaring_bitmap_from_range(0, 0, true);
    roaring_bitmap_add_many(shard_bm, end - begin, &values[begin]);
    
    // In-place union into the result
    roaring_bitmap_or_inplace(result, shard_bm);
    roaring_bitmap_free(shard_bm);
  }

  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  roaring_bitmap_free(result);
  return cardinality;
}