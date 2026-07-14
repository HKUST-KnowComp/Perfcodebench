#include "interface.h"
#include <roaring.h>
#include <vector>
#include <cstdint>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring_bitmap_t* bitmap = roaring_create();
  for (std::size_t i = 0; i + 1 < offsets.size(); ++i) {
    uint32_t begin = offsets[i];
    uint32_t end = offsets[i+1];
    roaring_add_many_sorted(bitmap, &values[begin], end - begin);
  }
  uint64_t cardinality = roaring_get_cardinality(bitmap);
  roaring_free(bitmap);
  return cardinality;
}