#include "interface.h"

#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0) return 0;

  // Fast path for single sorted shard: count unique in O(n) without extra structures
  if (shard_count == 1) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    if (end <= begin) return 0;
    uint64_t cnt = 1;
    for (uint32_t i = begin + 1; i < end; ++i) {
      if (values[i] != values[i - 1]) ++cnt;
    }
    return cnt;
  }

  roaring_bitmap_t* rb = roaring_bitmap_create();
  if (!rb) return 0;

  const uint32_t* vptr = values.data();
  for (size_t shard = 0; shard < shard_count; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t len = (end > begin) ? (end - begin) : 0;
    if (len) {
      // Each shard is sorted, enabling efficient bulk ingestion.
      roaring_bitmap_add_many(rb, static_cast<size_t>(len), vptr + begin);
    }
  }

  uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}
