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
    shard_bitmap.andSelf(values.data() + begin, end - begin);
    union_bitmap.orSelf(shard_bitmap);
  }
  return union_bitmap.cardinality();
}