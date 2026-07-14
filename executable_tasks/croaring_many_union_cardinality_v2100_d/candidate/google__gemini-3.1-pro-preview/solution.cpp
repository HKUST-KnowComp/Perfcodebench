#include "interface.h"
#include <roaring.h>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  std::size_t num_shards = offsets.size() - 1;
  std::vector<roaring_bitmap_t*> bitmaps(num_shards);
  
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    bitmaps[shard] = roaring_bitmap_of_ptr(end - begin, values.data() + begin);
  }
  
  roaring_bitmap_t* union_bitmap = roaring_bitmap_or_many(num_shards, (const roaring_bitmap_t**)bitmaps.data());
  
  uint64_t card = roaring_bitmap_get_cardinality(union_bitmap);
  
  roaring_bitmap_free(union_bitmap);
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    roaring_bitmap_free(bitmaps[shard]);
  }
  
  return card;
}
