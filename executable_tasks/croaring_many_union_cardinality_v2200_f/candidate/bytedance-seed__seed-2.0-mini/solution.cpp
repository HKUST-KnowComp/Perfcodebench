#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* const bm = roaring_bitmap_create();
  if (bm == nullptr) {
    return 0;
  }

  const size_t num_shards = offsets.size();
  for (size_t i = 0; i + 1 < num_shards; ++i) {
    const uint32_t begin = offsets[i];
    const uint32_t end = offsets[i + 1];
    if (begin >= end) {
      continue;
    }
    roaring_bitmap_add_array(bm, &values[begin], end - begin);
  }

  const uint64_t result = roaring_bitmap_get_cardinality(bm);
  roaring_bitmap_free(bm);
  return result;
}