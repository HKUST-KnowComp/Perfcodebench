#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) {
    return 0;
  }

  roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
  if (!union_bitmap) {
    return 0;
  }

  // Iterate through each shard defined by the offsets
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    if (begin >= end || begin >= values.size()) {
      continue;
    }

    // Ensure we don't read past the end of the values vector
    uint32_t actual_end = std::min(end, static_cast<uint32_t>(values.size()));

    // For each shard, we create a temporary bitmap and add its elements.
    // Since the input shards are already sorted (as per task description), 
    // we can optimize the insertion. However, roaring_bitmap_add is highly 
    // optimized for sequential/sorted data.
    for (uint32_t i = begin; i < actual_end; ++i) {
      roaring_bitmap_add(union_bitmap, values[i]);
    }
  }

  uint64_t cardinality = roaring_bitmap_cardinality(union_bitmap);
  roaring_bitmap_destroy(union_bitmap);

  return cardinality;
}