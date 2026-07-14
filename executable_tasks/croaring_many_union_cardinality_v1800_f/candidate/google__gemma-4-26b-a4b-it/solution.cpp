#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) {
    return 0;
  }

  roaring::Roaring union_bitmap;

  // Iterate through each shard defined by the offsets
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    if (begin >= end) continue;

    // Create a temporary bitmap for the current shard
    // Since the input values are sorted within shards (implied by typical roaring usage patterns)
    // or at least provided as a contiguous range, we can optimize.
    // However, the task implies 'values' contains the actual data.
    // We build a bitmap for the current shard.
    roaring::Roaring shard_bitmap;
    for (uint32_t i = begin; i < end; ++i) {
      shard_bitmap.add(values[i]);
    }
    
    // Perform the union
    union_bitmap |= shard_bitmap;
  }

  // Handle the last shard if offsets doesn't cover the full range of values
  // But based on the baseline, the loop 'shard + 1 < offsets.size()' is the logic to follow.
  // The baseline logic only processes shards defined by pairs in offsets.

  return static_cast<uint64_t>(union_bitmap.cardinality());
}