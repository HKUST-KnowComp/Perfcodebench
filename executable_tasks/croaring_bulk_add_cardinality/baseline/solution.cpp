#include "interface.h"

#include <unordered_set>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  std::unordered_set<uint32_t> unique_values;
  unique_values.insert(values.begin(), values.end());
  return static_cast<uint64_t>(unique_values.size());
}
