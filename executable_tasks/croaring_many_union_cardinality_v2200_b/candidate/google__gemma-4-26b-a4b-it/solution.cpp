#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) {
    return 0;
  }

  std::vector<roaring_bitmap_t*> bitmaps;
  // Pre-allocate to avoid reallocations
  bitmaps.reserve(offsets.size() - 1);

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    
    if (begin >= end) continue;

    roaring_bitmap_t* rb = roaring_bitmap_create();
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(rb, values[i]);
    }
    bitmaps.push_back(rb);
  }

  uint64_t total_cardinality = 0;
  if (!bitmaps.empty()) {
    // roaring_bitmap_union_cardinality is optimized for multiple bitmaps
    // However, the standard API often takes a single bitmap. 
    // For many-way union, we can use a divide and conquer approach or 
    // the specific roaring_bitmap_union function if available.
    // Given the constraints, we will perform a sequential union into a result bitmap.
    
    roaring_bitmap_t* result = roaring_bitmap_create();
    for (auto rb : bitmaps) {
      roaring_bitmap_or(result, result, rb);
      roaring_bitmap_destroy(rb);
    }
    total_cardinality = roaring_bitmap_cardinality(result);
    roaring_bitmap_destroy(result);
  }

  return total_cardinality;
}