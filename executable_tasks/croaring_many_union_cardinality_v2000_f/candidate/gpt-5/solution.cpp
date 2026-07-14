#include "interface.h"

#include <vector>
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) return 0;

  roaring_bitmap_t* rb = roaring_bitmap_create();
  if (!rb) return 0;

  const uint32_t* base = values.data();
  const size_t shard_count = offsets.size() - 1;
  for (size_t shard = 0; shard < shard_count; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    uint32_t len = end - begin;
    if (len == 0) continue;
    const uint32_t* ptr = base + begin;
    roaring_bitmap_add_many(rb, len, ptr);
  }

  uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}
