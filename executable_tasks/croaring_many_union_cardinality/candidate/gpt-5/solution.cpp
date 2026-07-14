#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) {
    return 0;
  }
  uint32_t total = offsets.back();
  if (total == 0) {
    return 0;
  }

  roaring_bitmap_t* rbm = roaring_bitmap_create();
  if (!rbm) {
    return 0; // Fallback: allocation failed
  }

  roaring_bitmap_add_many(rbm, static_cast<size_t>(total), values.data());
  uint64_t card = roaring_bitmap_get_cardinality(rbm);
  roaring_bitmap_free(rbm);
  return card;
}
