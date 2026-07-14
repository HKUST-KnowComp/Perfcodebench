#include "interface.h"

#include <roaring.h>

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0 || values.empty()) {
    return 0;
  }

  // Fast path for a single sorted shard: cardinality is just the number of
  // distinct adjacent values.
  if (shard_count == 1) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    if (begin >= end || begin >= values.size()) {
      return 0;
    }
    const uint32_t clamped_end =
        end > values.size() ? static_cast<uint32_t>(values.size()) : end;
    if (begin >= clamped_end) {
      return 0;
    }
    uint64_t count = 1;
    uint32_t prev = values[begin];
    for (uint32_t i = begin + 1; i < clamped_end; ++i) {
      const uint32_t v = values[i];
      count += static_cast<uint64_t>(v != prev);
      prev = v;
    }
    return count;
  }

  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(shard_count);

  for (std::size_t shard = 0; shard < shard_count; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];

    if (begin > values.size()) begin = static_cast<uint32_t>(values.size());
    if (end > values.size()) end = static_cast<uint32_t>(values.size());
    if (begin > end) begin = end;

    roaring_bitmap_t* bm = roaring_bitmap_create();
    if (begin < end) {
      // Input shards are sorted; use the bulk sorted loader for best build
      // performance and compact containers.
      roaring_bitmap_add_many_sorted(bm, end - begin, values.data() + begin);
      roaring_bitmap_run_optimize(bm);
    }
    bitmaps.push_back(bm);
  }

  uint64_t result = 0;
  if (!bitmaps.empty()) {
    roaring_bitmap_t* uni = roaring_bitmap_or_many(
        static_cast<uint32_t>(bitmaps.size()), bitmaps.data());
    result = uni ? roaring_bitmap_get_cardinality(uni) : 0;
    if (uni) {
      roaring_bitmap_free(uni);
    }
  }

  for (roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(bm);
  }

  return result;
}
