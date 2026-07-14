#include "interface.h"
#include <roaring/roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t num_shards = offsets.empty() ? 0 : offsets.size() - 1;
  if (num_shards == 0) return 0;

  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  for (size_t i = 0; i < num_shards; ++i) {
    const uint32_t begin = offsets[i];
    const uint32_t end   = offsets[i + 1];
    const uint32_t* ptr   = values.data() + begin;
    const size_t count    = end - begin;
    bitmaps.push_back(roaring_bitmap_of_ptr(count, ptr));
  }

  roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards, bitmaps.data());
  const uint64_t card = roaring_bitmap_get_cardinality(result);

  roaring_bitmap_free(result);
  for (roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(bm);
  }
  return card;
}