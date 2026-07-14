#include "interface.h"
#include "roaring.h"

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  if (starts.empty()) return 0;

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) return 0;

  for (std::size_t i = 0; i < starts.size(); ++i) {
    uint32_t start = starts[i];
    uint32_t len = lengths[i];
    if (len == 0) continue;
    
    // CRoaring add_range is [start, end), which matches our [begin, begin + length)
    roaring_bitmap_add_range(bitmap, start, start + len);
  }

  uint64_t cardinality = roaring_bitmap_cardinality(bitmap);
  roaring_bitmap_destroy(bitmap);
  return cardinality;
}