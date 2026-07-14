#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2 || values.empty()) {
    return 0;
  }
  roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
  const uint32_t* values_ptr = values.data();
  for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin >= end || end > values.size()) {
      continue;
    }
    roaring_bitmap_add_many(union_bitmap, end - begin, values_ptr + begin);
  }
  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);
  roaring_bitmap_free(union_bitmap);
  return cardinality;
}