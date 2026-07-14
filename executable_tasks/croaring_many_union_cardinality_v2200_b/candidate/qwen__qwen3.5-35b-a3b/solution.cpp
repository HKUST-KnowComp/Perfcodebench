#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    uint32_t prev = 0;
    bool first = true;
    for (uint32_t i = begin; i < end; ++i) {
      uint32_t val = values[i];
      if (first || val != prev) {
        roaring_add(bitmap, val);
        prev = val;
        first = false;
      }
    }
  }
  uint64_t cardinality = roaring_get_cardinality(bitmap);
  roaring_free(bitmap);
  return cardinality;
}