#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  std::vector<RoaringBitmap*> bitmaps;
  bitmaps.reserve(offsets.size() - 1);

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    if (begin >= end) continue;

    RoaringBitmap* rb = new RoaringBitmap();
    // We use add_many if available or a loop to populate the bitmap.
    // Since the input is sorted within shards (implied by 'sorted shards' in title),
    // we can optimize, but standard add is safe.
    for (uint32_t i = begin; i < end; ++i) {
      rb->add(values[i]);
    }
    bitmaps.push_back(rb);
  }

  if (bitmaps.empty()) {
    return 0;
  }

  RoaringBitmap* result = nullptr;
  if (bitmaps.size() == 1) {
    result = bitmaps[0];
  } else {
    // roaring_bitmap_union is highly optimized for multiple bitmaps
    result = roaring_bitmap_union(bitmaps.size(), bitmaps.data());
  }

  uint64_t cardinality = result->cardinality();

  // Cleanup
  if (bitmaps.size() > 1) {
    // If we created a new bitmap via union, we must delete it
    // Note: roaring_bitmap_union returns a new pointer.
    // We also need to delete the individual bitmaps.
    for (auto rb : bitmaps) {
      delete rb;
    }
    delete result;
  } else {
    // If only one bitmap, result is just the pointer in the vector
    delete bitmaps[0];
  }

  return cardinality;
}