#include "interface.h"

#include <unordered_set>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  std::unordered_set<uint32_t> values;
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint32_t begin = starts[i];
    const uint32_t end = begin + lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      values.insert(v);
    }
  }
  return static_cast<uint64_t>(values.size());
}
