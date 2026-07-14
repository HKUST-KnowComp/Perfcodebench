#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) return 0;

  // Create a bitmap and pre-allocate to avoid repeated reallocations.
  roaring_bitmap_t* rb = roaring_bitmap_create();
  if (!rb) return 0;

  for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end   = offsets[shard + 1];
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(rb, values[i]);
    }
  }

  const uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}