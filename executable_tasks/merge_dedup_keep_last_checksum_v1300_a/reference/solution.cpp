#include "interface.h"

#include <utility>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  std::vector<std::pair<uint32_t, uint32_t>> out;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    out.reserve(left_keys.size() + right_keys.size());
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < left_keys.size() && j < right_keys.size()) {
      if (left_keys[i] < right_keys[j]) {
        out.emplace_back(left_keys[i], left_values[i]);
        ++i;
      } else if (right_keys[j] < left_keys[i]) {
        out.emplace_back(right_keys[j], right_values[j]);
        ++j;
      } else {
        out.emplace_back(right_keys[j], right_values[j]);
        ++i;
        ++j;
      }
    }
    while (i < left_keys.size()) {
      out.emplace_back(left_keys[i], left_values[i]);
      ++i;
    }
    while (j < right_keys.size()) {
      out.emplace_back(right_keys[j], right_values[j]);
      ++j;
    }
    hash = 1469598103934665603ULL;
    for (const auto& kv : out) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  return hash;
}
