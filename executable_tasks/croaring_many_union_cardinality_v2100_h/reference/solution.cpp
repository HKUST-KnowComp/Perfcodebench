#include "interface.h"

#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  std::vector<roaring_bitmap_t*> bitmaps;
  std::vector<const roaring_bitmap_t*> bitmap_ptrs;
  bitmaps.reserve(offsets.size() - 1);
  bitmap_ptrs.reserve(offsets.size() - 1);
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    roaring_bitmap_add_many(bitmap, end - begin, values.data() + begin);
    bitmaps.push_back(bitmap);
    bitmap_ptrs.push_back(bitmap);
  }
  roaring_bitmap_t* merged =
      roaring_bitmap_or_many_heap(bitmap_ptrs.size(), bitmap_ptrs.data());
  const uint64_t total = roaring_bitmap_get_cardinality(merged);
  roaring_bitmap_free(merged);
  for (roaring_bitmap_t* bitmap : bitmaps) {
    roaring_bitmap_free(bitmap);
  }
  return total;
}
