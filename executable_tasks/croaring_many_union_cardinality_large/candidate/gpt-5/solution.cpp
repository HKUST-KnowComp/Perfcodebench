#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shards == 0) return 0;

  // Fast path for single shard: count uniques in sorted segment
  if (shards == 1) {
    uint32_t begin = offsets[0];
    uint32_t end = offsets[1];
    if (end <= begin) return 0;
    const uint32_t* ptr = values.data() + begin;
    size_t len = static_cast<size_t>(end - begin);
    uint64_t cnt = 1;
    uint32_t prev = ptr[0];
    for (size_t i = 1; i < len; ++i) {
      uint32_t v = ptr[i];
      if (v != prev) {
        ++cnt;
        prev = v;
      }
    }
    return cnt;
  }

  roaring_bitmap_t* bm = roaring_bitmap_create();
  // Add each shard; CRoaring add_many is optimized for sorted arrays
  for (size_t shard = 0; shard < shards; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    if (end <= begin) continue;
    const uint32_t* ptr = values.data() + begin;
    size_t len = static_cast<size_t>(end - begin);
    roaring_bitmap_add_many(bm, len, ptr);
  }

  uint64_t result = roaring_bitmap_get_cardinality(bm);
  roaring_bitmap_free(bm);
  return result;
}
