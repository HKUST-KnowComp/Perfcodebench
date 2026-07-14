#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  CRoaring::Roaring unique_values;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    CRoaring::Roaring shard_values = CRoaring::Roaring::bitmapOf(end - begin, values.data() + begin);
    unique_values |= shard_values;
  }
  return unique_values.cardinality();
}