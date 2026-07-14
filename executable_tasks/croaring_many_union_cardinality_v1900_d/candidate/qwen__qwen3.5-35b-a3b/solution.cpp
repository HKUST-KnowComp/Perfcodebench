#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring::bitmap rb;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    for (uint32_t i = begin; i < end; ++i) {
      if (i > begin && values[i] == values[i-1]) {
        continue;
      }
      rb.add(values[i]);
    }
  }
  return rb.cardinality();
}