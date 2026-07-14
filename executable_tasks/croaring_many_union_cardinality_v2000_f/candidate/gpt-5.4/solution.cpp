#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size();
  if (shard_count <= 1) {
    return 0;
  }

  roaring_bitmap_t* acc = roaring_bitmap_create();
  if (!acc) {
    return 0;
  }

  for (std::size_t shard = 0; shard + 1 < shard_count; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t len = end - begin;
    if (len == 0) {
      continue;
    }

    roaring_bitmap_add_many(acc, len, values.data() + begin);
  }

  roaring_bitmap_run_optimize(acc);
  const uint64_t result = roaring_bitmap_get_cardinality(acc);
  roaring_bitmap_free(acc);
  return result;
}
