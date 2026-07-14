#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) return 0;

  croaring::Roaring bitmap;
  
  // The input is provided as shards. We iterate through each shard
  // and add its elements to the Roaring bitmap.
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    // Since the shards are sorted, we could potentially use 
    // add_range or other bulk methods, but for general 32-bit 
    // integers, add() is highly optimized in CRoaring.
    for (uint32_t i = begin; i < end; ++i) {
      bitmap.add(values[i]);
    }
  }

  return bitmap.cardinality();
}