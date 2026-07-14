#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size();
  if (shard_count <= 1) return 0;

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) return 0;

  for (std::size_t shard = 0; shard + 1 < shard_count; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;
    if (begin >= values.size()) continue;
    if (end > values.size()) end = static_cast<uint32_t>(values.size());

    const uint32_t* data = values.data() + begin;
    const uint32_t length = end - begin;

    // Shards are sorted; use the sorted bulk-add path for better throughput.
    roaring_bitmap_add_many(bitmap, length, data);
  }

  const uint64_t result = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return result;
}
