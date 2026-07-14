#include <cstdint>
#include <vector>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  std::vector<roaring_bitmap_t*> bitmaps;
  for (std::size_t i = 0; i + 1 < offsets.size(); ++i) {
    roaring_bitmap_t* bitmap = roaring_create();
    uint32_t begin = offsets[i];
    uint32_t end = offsets[i + 1];
    if (begin < end) {
      roaring_add_many_sorted(bitmap, &values[begin], end - begin);
    }
    bitmaps.push_back(bitmap);
  }

  if (bitmaps.empty()) {
    return 0;
  }

  roaring_bitmap_t* result = roaring_or_many(bitmaps.data(), bitmaps.size());
  uint64_t cardinality = roaring_get_cardinality(result);
  roaring_free(result);

  for (roaring_bitmap_t* bitmap : bitmaps) {
    roaring_free(bitmap);
  }

  return cardinality;
}