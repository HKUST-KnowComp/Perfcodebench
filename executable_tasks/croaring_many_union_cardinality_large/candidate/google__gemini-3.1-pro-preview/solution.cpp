#include "interface.h"
#include <roaring/roaring.h>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  std::size_t num_shards = offsets.size() - 1;
  std::vector<const roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);

  for (std::size_t shard = 0; shard < num_shards; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    if (end > begin) {
      bitmaps.push_back(roaring_bitmap_of_ptr(end - begin, values.data() + begin));
    }
  }

  if (bitmaps.empty()) {
    return 0;
  }

  roaring_bitmap_t* result = roaring_bitmap_or_many(bitmaps.size(), bitmaps.data());
  uint64_t card = roaring_bitmap_get_cardinality(result);

  roaring_bitmap_free(result);
  for (const roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(const_cast<roaring_bitmap_t*>(bm));
  }

  return card;
}
