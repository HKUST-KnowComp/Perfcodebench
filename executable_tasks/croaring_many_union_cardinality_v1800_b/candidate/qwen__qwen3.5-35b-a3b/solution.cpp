#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* result = roaring_bitmap_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t* shard_data = &values[offsets[shard]];
    std::size_t shard_size = offsets[shard + 1] - offsets[shard];
    if (shard_size == 0) continue;
    
    roaring_bitmap_t* shard_bitmap = roaring_create_from_sorted_array(
        shard_data, shard_size);
    roaring_or_inplace(result, shard_bitmap);
    roaring_bitmap_free(shard_bitmap);
  }
  
  uint64_t cardinality = roaring_bitmap_cardinality(result);
  roaring_bitmap_free(result);
  return cardinality;
}