#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  size_t num_shards = offsets.size() == 0 ? 0 : offsets.size() - 1;
  if (num_shards == 0) return 0;

  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(num_shards);
  for (size_t i = 0; i < num_shards; ++i) {
    uint32_t begin = offsets[i];
    uint32_t end   = offsets[i + 1];
    size_t count = end - begin;
    if (count > 0) {
      bitmaps.push_back(roaring_bitmap_of_ptr(count, &values[begin]));
    } else {
      bitmaps.push_back(roaring_bitmap_create());
    }
  }

  roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards,
      (const roaring_bitmap_t **)bitmaps.data());
  uint64_t card = roaring_bitmap_get_cardinality(result);

  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  roaring_bitmap_free(result);

  return card;
}