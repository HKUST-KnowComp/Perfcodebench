#include "interface.h"
#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  for (size_t i = 0; i < starts.size(); ++i) {
    uint32_t start = starts[i];
    uint32_t end = start + lengths[i];
    // roaring_bitmap_add_range adds [start, end) and gracefully handles end <= start (e.g., overflow)
    roaring_bitmap_add_range(r, start, end);
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}