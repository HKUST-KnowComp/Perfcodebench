#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t n = offsets.size();
  if (n <= 1) return 0;

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) return 0;

  for (std::size_t shard = 0; shard + 1 < n; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin >= end) continue;

    const uint32_t* data = values.data() + begin;
    const uint32_t count = end - begin;

    // Each shard is sorted, so CRoaring can ingest efficiently.
    roaring_bitmap_add_many(bitmap, count, data);
  }

  const uint64_t result = static_cast<uint64_t>(roaring_bitmap_get_cardinality(bitmap));
  roaring_bitmap_free(bitmap);
  return result;
}
