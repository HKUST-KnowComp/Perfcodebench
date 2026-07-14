#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* result = roaring_bitmap_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    uint32_t count = end - begin;
    if (count == 0) continue;
    roaring_bitmap_t* shard_bitmap = roaring_bitmap_from_sorted_array(&values[begin], count);
    roaring_bitmap_or_inplace(result, shard_bitmap);
    roaring_bitmap_free(shard_bitmap);
  }
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  roaring_bitmap_free(result);
  return cardinality;
}