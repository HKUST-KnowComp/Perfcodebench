#include "interface.h"

#include <unordered_set>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  std::unordered_set<uint32_t> unique_values;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    for (uint32_t i = begin; i < end; ++i) {
      unique_values.insert(values[i]);
    }
  }
  return static_cast<uint64_t>(unique_values.size());
}
