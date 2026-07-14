#include "interface.h"

#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.size() < 2) return 0;
  size_t nshards = offsets.size() - 1;
  std::vector<roaring_bitmap_t*> bms;
  bms.reserve(nshards);
  for (size_t i = 0; i < nshards; ++i) {
    uint32_t begin = offsets[i];
    uint32_t end = offsets[i + 1];
    if (end > begin) {
      bms.push_back(roaring_bitmap_from_sorted_array(
          values.data() + begin, end - begin));
    }
  }
  if (bms.empty()) return 0;
  if (bms.size() == 1) {
    uint64_t card = roaring_bitmap_cardinality(bms[0]);
    roaring_bitmap_free(bms[0]);
    return card;
  }
  std::vector<const roaring_bitmap_t*> cptr(bms.size());
  for (size_t i = 0; i < bms.size(); ++i) cptr[i] = bms[i];
  roaring_bitmap_t* result =
      roaring_bitmap_or_many(bms.size(), cptr.data());
  uint64_t card = roaring_bitmap_cardinality(result);
  roaring_bitmap_free(result);
  for (auto* b : bms) roaring_bitmap_free(b);
  return card;
}