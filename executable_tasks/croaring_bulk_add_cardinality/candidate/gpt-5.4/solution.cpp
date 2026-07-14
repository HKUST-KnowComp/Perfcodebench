#include "interface.h"

#include <roaring.h>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  const size_t n = values.size();
  if (n == 0) return 0;

  roaring_bitmap_t* bitmap = roaring_bitmap_create();
  if (!bitmap) return 0;

  // Input is sorted per task description, so bulk sorted insertion is optimal.
  roaring_bitmap_add_many(bitmap, static_cast<uint32_t>(n), values.data());
  const uint64_t card = roaring_bitmap_get_cardinality(bitmap);
  roaring_bitmap_free(bitmap);
  return card;
}
