#pragma once

#include <cstdint>
#include <vector>
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    roaring_bitmap_add_many(bitmap, &values[begin], end - begin);
  }
  uint64_t cardinality = roaring_bitmap_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return cardinality;
}