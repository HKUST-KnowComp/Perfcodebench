#include "interface.h"

#include <cstdint>
#include <vector>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }

  // Fast path for a single shard: input is sorted, just count unique values.
  if (offsets.size() == 2) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    if (end <= begin) return 0;
    uint64_t count = 1;
    uint32_t prev = values[begin];
    for (uint32_t i = begin + 1; i < end; ++i) {
      uint32_t v = values[i];
      if (v != prev) {
        ++count;
        prev = v;
      }
    }
    return count;
  }

  roaring_bitmap_t* bm = roaring_bitmap_create();
  if (!bm) return 0;

  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    const uint32_t n = (end > begin) ? (end - begin) : 0u;
    if (n == 0) continue;
    const uint32_t* ptr = values.data() + begin;
    roaring_bitmap_add_many(bm, n, ptr);
  }

  uint64_t card = roaring_bitmap_get_cardinality(bm);
  roaring_bitmap_free(bm);
  return card;
}
