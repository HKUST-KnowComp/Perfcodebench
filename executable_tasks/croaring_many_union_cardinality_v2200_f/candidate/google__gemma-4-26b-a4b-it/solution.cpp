#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  size_t num_shards = offsets.size() - 1;
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);

  for (size_t shard = 0; shard < num_shards; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    
    roaring_bitmap_t* rb = roaring_bitmap_create();
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(rb, values[i]);
    }
    bitmaps.push_back(rb);
  }

  uint64_t result = 0;
  if (!bitmaps.empty()) {
    // roaring_bitmap_union_cardinality is optimized for multiple bitmaps
    // However, the standard API often takes a single bitmap or requires manual reduction.
    // To ensure maximum compatibility with the provided roaring.h interface:
    roaring_bitmap_t* union_rb = roaring_bitmap_create();
    for (auto rb : bitmaps) {
      roaring_bitmap_or(union_rb, union_rb, rb);
      roaring_bitmap_destroy(rb);
    }
    result = roaring_bitmap_cardinality(union_rb);
    roaring_bitmap_destroy(union_rb);
  }

  return result;
}