#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  RoaringBitmap* result = RoaringBitmapCreate();
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    const uint32_t* data = &values[begin];
    size_t size = end - begin;
    RoaringBitmap* shard_bitmap = RoaringBitmapCreateFromSortedArray(data, size);
    RoaringBitmapOr(result, shard_bitmap);
    RoaringBitmapDelete(shard_bitmap);
  }
  uint64_t cardinality = RoaringBitmapCardinality(result);
  RoaringBitmapDelete(result);
  return cardinality;
}