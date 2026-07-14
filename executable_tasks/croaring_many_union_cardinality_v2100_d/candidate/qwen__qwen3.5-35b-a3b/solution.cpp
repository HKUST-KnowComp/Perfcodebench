#include <vector>
#include <cstdint>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  roaring::Roaring result;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    std::size_t begin = offsets[shard];
    std::size_t end = offsets[shard + 1];
    roaring::Roaring shard_rb;
    shard_rb.add_sorted_array(&values[begin], end - begin);
    result |= shard_rb;
  }
  return result.cardinality();
}