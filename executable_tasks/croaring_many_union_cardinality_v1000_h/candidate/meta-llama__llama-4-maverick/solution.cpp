#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  Roaring unique_values;
  unique_values.setCopyOnWrite(true);
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    Roaring shard_values = Roaring::bitmapOf(end - begin, &values[begin]);
    shard_values.setCopyOnWrite(true);
    unique_values |= shard_values;
  }
  return unique_values.cardinality();
}