#include <cstdint>
#include <vector>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring::RoaringBitmap bitmap;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    bitmap.add_many(values.data() + begin, end - begin);
  }
  return bitmap.cardinality();
}