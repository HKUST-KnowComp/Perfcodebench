#include "interface.h"
#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t *r = roaring_bitmap_create();
  const size_t n = starts.size();
  for (size_t i = 0; i < n; ++i) {
    uint64_t begin = starts[i];
    uint64_t end = begin + lengths[i];
    if (end > begin) {
      roaring_bitmap_add_range(r, begin, end);
    }
  }
  uint64_t card = roaring_bitmap_get_cardinality(r);
  roaring_bitmap_free(r);
  return card;
}
