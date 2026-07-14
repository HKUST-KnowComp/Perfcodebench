#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) {
    return 0;
  }

  // Iterate through each shard defined by the offsets
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    // If the shard is empty, skip it
    if (begin >= end) continue;

    // For each shard, we can build a temporary bitmap or add elements.
    // Since the input is a flat vector of values, we iterate through the range.
    // To maximize performance with CRoaring, we add elements to the main bitmap.
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(bitmap, values[i]);
    }
  }

  uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_destroy(bitmap);

  return cardinality;
}