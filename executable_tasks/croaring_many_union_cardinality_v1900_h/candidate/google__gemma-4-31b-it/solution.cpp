#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) return 0;

  croaring::Roaring bitmap;
  
  // Iterate through each shard defined by the offsets
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    // Roaring bitmaps provide a fast way to add ranges of values
    // Since the input shards are sorted, we can potentially use specialized methods,
    // but add_array is generally very efficient.
    if (begin < end) {
      bitmap.add_array(values.data() + begin, end - begin);
    }
  }

  return bitmap.cardinality();
}