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
    for (uint32_t i = begin; i < end; ++i) {
      rb->add(values[i]);
    }
    bitmaps.push_back(rb);
  }

  if (bitmaps.empty()) {
    return 0;
  }

  RoaringBitmap union_result;
  // Using the library's ability to union multiple bitmaps efficiently
  // Note: In a production environment, one might use RoaringBitmap_union_many
  // but for standard CRoaring API compatibility, we iterate or use the provided aggregate functions.
  
  // For the purpose of this task, we perform a standard union.
  // To optimize for 'many' shards, we use the library's internal optimizations if available.
  // Since we must provide a complete solution, we implement the union loop.
  
  union_result = *bitmaps[0];
  for (std::size_t i = 1; i < bitmaps.size(); ++i) {
    union_result.or_with(*bitmaps[i]);
  }

  uint64_t cardinality = union_result.get_cardinality();

  for (auto rb : bitmaps) {
    delete rb;
  }

  return cardinality;
}