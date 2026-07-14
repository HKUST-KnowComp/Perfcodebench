#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty() || offsets.size() < 2) {
    return 0;
  }

  roaringbitmap_t* union_bitmap = roaringbitmap_create();
  
  // Iterate through each shard defined by the offsets
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t start_idx = offsets[shard];
    uint32_t end_idx = offsets[shard + 1];
    
    if (start_idx >= end_idx || start_idx >= values.size()) {
      continue;
    }
    
    // Ensure we don't read past the end of the values vector
    uint32_t actual_end = (end_idx > values.size()) ? values.size() : end_idx;
    
    // Create a temporary bitmap for the current shard to allow for efficient batch addition
    // However, for maximum performance with CRoaring, we can use roaringbitmap_add
    // or build a temporary bitmap and use roaringbitmap_or_inplace.
    // Given the input is sorted within shards (implied by 'sorted shards' in title),
    // we can optimize. If not strictly sorted, we add elements.
    
    for (uint32_t i = start_idx; i < actual_end; ++i) {
      roaringbitmap_add(union_bitmap, values[i]);
    }
  }

  uint64_t cardinality = roaringbitmap_cardinality(union_bitmap);
  roaringbitmap_destroy(union_bitmap);

  return cardinality;
}