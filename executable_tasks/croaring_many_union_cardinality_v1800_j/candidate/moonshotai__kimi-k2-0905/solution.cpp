#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t *rb = roaring_bitmap_create();
  const size_t shards = offsets.size() - 1;
  for (size_t s = 0; s < shards; ++s) {
    const uint32_t begin = offsets[s];
    const uint32_t end   = offsets[s + 1];
    const uint32_t run_len = end - begin;
    if (run_len) {
      roaring_bitmap_add_many(rb, run_len, &values[begin]);
    }
  }
  uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}