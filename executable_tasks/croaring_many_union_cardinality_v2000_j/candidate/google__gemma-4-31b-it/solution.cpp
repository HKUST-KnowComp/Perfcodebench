#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  // Use CRoaring for high-performance set operations
  roaring_bitmap_t* rb = roaring_bitmap_create();
  if (!rb) return 0;

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    // Since the shards are sorted, we can use the bulk add function
    // which is significantly faster than individual insertions.
    if (begin < end) {
      roaring_bitmap_add_array(rb, (uint32_t*)(&values[begin]), end - begin);
    }
  }

  uint64_t cardinality = roaring_bitmap_cardinality(rb);
  roaring_bitmap_destroy(rb);
  return cardinality;
}