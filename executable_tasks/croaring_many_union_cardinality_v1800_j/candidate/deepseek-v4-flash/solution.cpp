#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  roaring_bulk_insert_t bulk;  // no initialization needed
  for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    uint32_t start = offsets[shard];
    uint32_t count = offsets[shard + 1] - start;
    if (count == 0) continue;
    roaring_bitmap_add_bulk(r, &bulk, &values[start], count);
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}