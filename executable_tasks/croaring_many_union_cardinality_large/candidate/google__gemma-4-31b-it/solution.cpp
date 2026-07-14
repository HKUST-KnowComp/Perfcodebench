#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) return 0;

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) return 0;

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(bitmap, values[i]);
    }
  }

  uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_destroy(bitmap);
  return cardinality;
}