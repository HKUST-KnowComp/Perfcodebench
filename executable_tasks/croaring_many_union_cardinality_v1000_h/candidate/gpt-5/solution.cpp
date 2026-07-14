#include "interface.h"

#include "roaring.h"

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const uint32_t total = offsets.empty() ? 0u : offsets.back();
  if (total == 0) return 0;

  roaring_bitmap_t* rb = roaring_bitmap_create();
  roaring_bitmap_add_many(rb, total, values.data());
  const uint64_t card = roaring_bitmap_get_cardinality(rb);
  roaring_bitmap_free(rb);
  return card;
}
