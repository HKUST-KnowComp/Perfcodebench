#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0) return 0;

  roaring_bitmap_t* acc = roaring_bitmap_create();
  if (!acc) return 0;

  for (std::size_t shard = 0; shard < shard_count; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;

    const uint32_t* data = values.data() + begin;
    const size_t length = static_cast<size_t>(end - begin);

    roaring_bitmap_t* bm = roaring_bitmap_create();
    if (!bm) {
      roaring_bitmap_free(acc);
      return 0;
    }

    roaring_bitmap_add_many(bm, length, data);
    roaring_bitmap_run_optimize(bm);
    roaring_bitmap_or_inplace(acc, bm);
    roaring_bitmap_free(bm);
  }

  const uint64_t result = roaring_bitmap_get_cardinality(acc);
  roaring_bitmap_free(acc);
  return result;
}
