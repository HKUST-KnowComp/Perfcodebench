#include "interface.h"
#include "roaring.h"
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t num_shards = offsets.size();
  if (num_shards <= 1) return 0;

  // Build the first shard as the initial accumulator
  const uint32_t* first_begin = &values[offsets[0]];
  const size_t first_size = offsets[1] - offsets[0];
  roaring_bitmap_t* accum = roaring_bitmap_of_ptr(first_size, first_begin);

  // Iterate over the remaining shards and union into accumulator
  for (size_t i = 1; i + 1 < num_shards; ++i) {
    const uint32_t* begin = &values[offsets[i]];
    const size_t size = offsets[i + 1] - offsets[i];
    roaring_bitmap_t* bmp = roaring_bitmap_of_ptr(size, begin);
    roaring_bitmap_or_inplace(accum, bmp);
    roaring_bitmap_free(bmp);
  }

  uint64_t card = roaring_bitmap_get_cardinality(accum);
  roaring_bitmap_free(accum);
  return card;
}