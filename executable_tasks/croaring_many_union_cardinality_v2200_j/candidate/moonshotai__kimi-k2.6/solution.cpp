#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *bitmap = roaring_bitmap_create();
  if (!bitmap) {
    return 0;
  }

  roaring_bulk_context_t ctx = {};
  const uint32_t *data = values.data();

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t *p = data + begin;
    const uint32_t *pend = data + end;
    for (; p != pend; ++p) {
      roaring_bitmap_add_bulk(bitmap, &ctx, *p);
    }
  }

  uint64_t result = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return result;
}
