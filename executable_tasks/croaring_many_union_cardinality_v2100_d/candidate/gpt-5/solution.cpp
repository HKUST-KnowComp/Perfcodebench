#include "interface.h"

#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  (void)offsets; // shards are individually sorted, but union equals set of all values
  roaring_bitmap_t* rb = roaring_bitmap_create();
  if (!rb) return 0;

  if (!values.empty()) {
    roaring_bitmap_add_many(rb, values.size(), values.data());
  }

  uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}
