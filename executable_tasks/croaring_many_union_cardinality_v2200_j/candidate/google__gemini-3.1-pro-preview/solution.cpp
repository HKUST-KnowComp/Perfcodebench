#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) return 0;
  size_t num_shards = offsets.size() - 1;
  if (num_shards == 0) return 0;

  std::vector<const roaring_bitmap_t*> bitmaps(num_shards);
  for (size_t i = 0; i < num_shards; ++i) {
    uint32_t begin = offsets[i];
    uint32_t end = offsets[i + 1];
    bitmaps[i] = roaring_bitmap_of_ptr(end - begin, values.data() + begin);
  }

  roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards, bitmaps.data());
  uint64_t card = roaring_bitmap_get_cardinality(result);

  roaring_bitmap_free(result);
  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(const_cast<roaring_bitmap_t*>(bitmaps[i]));
  }

  return card;
}
