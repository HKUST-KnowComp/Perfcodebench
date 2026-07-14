#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  RoaringBitmap union_bitmap;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    RoaringBitmap shard_bitmap;
    shard_bitmap.addRange(begin, end);
    for (uint32_t i = begin; i < end; ++i) {
      shard_bitmap.add(values[i]);
    }
    union_bitmap |= shard_bitmap;
  }
  return union_bitmap.cardinality();
}