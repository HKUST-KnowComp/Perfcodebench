#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? (offsets.size() - 1) : 0;
  if (shard_count == 0) return 0;

  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(shard_count);

  for (std::size_t shard = 0; shard < shard_count; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t len = end - begin;

    roaring_bitmap_t* bm = roaring_bitmap_create();
    if (len != 0) {
      roaring_bitmap_add_many_sorted(bm, len, values.data() + begin);
      roaring_bitmap_run_optimize(bm);
    }
    bitmaps.push_back(bm);
  }

  roaring_bitmap_t* result = roaring_bitmap_or_many_heap(
      static_cast<uint32_t>(bitmaps.size()), bitmaps.data());

  uint64_t answer = result ? roaring_bitmap_get_cardinality(result) : 0;

  if (result) {
    roaring_bitmap_free(result);
  }
  for (roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(bm);
  }

  return answer;
}
