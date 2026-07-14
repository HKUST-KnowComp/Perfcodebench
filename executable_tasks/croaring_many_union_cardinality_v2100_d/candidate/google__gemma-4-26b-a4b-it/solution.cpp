#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  std::vector<roaring_bitmap_t*> bitmaps;
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

  if (bitmaps.empty()) {
    return 0;
  }

  roaring_bitmap_t* union_bitmap = roaring_bitmap_union(bitmaps.size(), bitmaps.data());
  uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);

  // Cleanup
  for (auto rb : bitmaps) {
    roaring_bitmap_destroy(rb);
  }
  roaring_bitmap_destroy(union_bitmap);

  return cardinality;
}