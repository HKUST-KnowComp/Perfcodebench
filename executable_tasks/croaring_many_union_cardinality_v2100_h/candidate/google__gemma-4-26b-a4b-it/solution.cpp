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
    // Since the input values are sorted within shards (implied by context/task), 
    // we can use add_many for better performance.
    // If they aren't strictly sorted, add_many still works but is optimized for runs.
    rb->add_many(values.begin() + begin, values.begin() + end);
    bitmaps.push_back(rb);
  }

  if (bitmaps.empty()) {
    return 0;
  }

  RoaringBitmap* union_result = nullptr;
  if (bitmaps.size() == 1) {
    union_result = bitmaps[0];
    bitmaps[0] = nullptr; // Prevent double deletion
  } else {
    union_result = roaring_bitmap_union_many(bitmaps.data(), static_cast<uint32_t>(bitmaps.size()));
  }

  uint64_t cardinality = static_cast<uint64_t>(union_result->cardinality());

  // Cleanup
  for (auto* rb : bitmaps) {
    delete rb;
  }
  delete union_result;

  return cardinality;
}