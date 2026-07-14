#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t num_shards = offsets.size() - 1;
  if (num_shards == 0) return 0;
  
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin < end) {
      bitmaps.push_back(roaring_bitmap_of_ptr(end - begin, values.data() + begin));
    }
  }
  
  if (bitmaps.empty()) return 0;
  
  uint64_t card = roaring_bitmap_or_many_cardinality(
      bitmaps.size(),
      (const roaring_bitmap_t**)bitmaps.data());
  
  for (roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(bm);
  }
  return card;
}
