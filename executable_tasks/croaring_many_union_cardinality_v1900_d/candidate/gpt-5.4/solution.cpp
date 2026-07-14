#include "interface.h"

#include <roaring.h>

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size();
  if (shard_count <= 1) return 0;

  // Fast path: no values.
  if (values.empty()) return 0;

  // Build one bitmap per non-empty shard using the sorted-input bulk loader.
  // We skip empty shards to reduce allocation and union overhead.
  std::vector<roaring_bitmap_t*> bitmaps;
  bitmaps.reserve(shard_count - 1);

  for (std::size_t shard = 0; shard + 1 < shard_count; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];

    if (begin >= end) continue;
    if (begin >= values.size()) continue;
    if (end > values.size()) end = static_cast<uint32_t>(values.size());
    if (begin >= end) continue;

    const uint32_t* data = values.data() + begin;
    const uint64_t length = static_cast<uint64_t>(end - begin);

    // Input shards are sorted; use the optimized constructor.
    roaring_bitmap_t* bm = roaring_bitmap_of_ptr(length, data);
    if (bm != nullptr) {
      bitmaps.push_back(bm);
    }
  }

  if (bitmaps.empty()) return 0;

  uint64_t result = 0;
  if (bitmaps.size() == 1) {
    result = roaring_bitmap_get_cardinality(bitmaps[0]);
    roaring_bitmap_free(bitmaps[0]);
    return result;
  }

  // Use CRoaring's many-way OR cardinality directly to avoid materializing
  // the final union when only the cardinality is needed.
  result = roaring_bitmap_or_many_heap_cardinality(
      static_cast<uint32_t>(bitmaps.size()), bitmaps.data());

  for (roaring_bitmap_t* bm : bitmaps) {
    roaring_bitmap_free(bm);
  }

  return result;
}
