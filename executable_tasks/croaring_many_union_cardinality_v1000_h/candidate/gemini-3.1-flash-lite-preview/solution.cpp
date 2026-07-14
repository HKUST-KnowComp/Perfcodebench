#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) return 0;

  roaring_bitmap_t* rb = roaring_bitmap_create();
  
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    if (begin < end) {
      // roaring_bitmap_add_many is efficient for adding sorted sequences
      roaring_bitmap_add_many(rb, end - begin, &values[begin]);
    }
  }

  uint64_t cardinality = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return cardinality;
}