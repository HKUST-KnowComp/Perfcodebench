#include <cstdint>
#include <vector>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    uint32_t count = end - begin;
    const uint32_t* data = &values[begin];
    roaring_add_array(bitmap, count, data);
  }
  uint64_t cardinality = roaring_get_cardinality(bitmap);
  roaring_delete(bitmap);
  return cardinality;
}