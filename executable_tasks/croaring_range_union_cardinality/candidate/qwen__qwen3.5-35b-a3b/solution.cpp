#include "interface.h"
#include "roaring.h"
#include <vector>
#include <cstdint>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  for (std::size_t i = 0; i < starts.size(); ++i) {
    uint32_t start = starts[i];
    uint32_t end = start + lengths[i];
    roaring_bitmap_add_range(bitmap, start, end);
  }
  uint64_t count = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return count;
}