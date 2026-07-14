#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* main_bitmap = roaring_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    uint32_t* shard_values = const_cast<uint32_t*>(&values[begin]);
    uint32_t shard_size = end - begin;
    if (shard_size == 0) continue;
    roaring_bitmap_t* shard_bitmap = roaring_create_from_sorted_array(shard_size, shard_values);
    roaring_or_inplace(main_bitmap, shard_bitmap);
    roaring_destroy(shard_bitmap);
  }
  uint64_t cardinality = roaring_get_cardinality(main_bitmap);
  roaring_destroy(main_bitmap);
  return cardinality;
}