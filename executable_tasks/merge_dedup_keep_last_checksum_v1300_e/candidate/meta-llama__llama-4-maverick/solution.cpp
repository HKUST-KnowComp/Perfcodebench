#include "interface.h"

#include <algorithm>
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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::pair<uint32_t, uint32_t>> out;
    out.reserve(left_keys.size() + right_keys.size());
    std::size_t left_idx = 0;
    std::size_t right_idx = 0;
    while (left_idx < left_keys.size() && right_idx < right_keys.size()) {
      if (left_keys[left_idx] < right_keys[right_idx]) {
        out.emplace_back(left_keys[left_idx], left_values[left_idx]);
        ++left_idx;
      } else if (left_keys[left_idx] > right_keys[right_idx]) {
        out.emplace_back(right_keys[right_idx], right_values[right_idx]);
        ++right_idx;
      } else {
        out.emplace_back(left_keys[left_idx], right_values[right_idx]);
        ++left_idx;
        ++right_idx;
      }
    }
    while (left_idx < left_keys.size()) {
      out.emplace_back(left_keys[left_idx], left_values[left_idx]);
      ++left_idx;
    }
    while (right_idx < right_keys.size()) {
      out.emplace_back(right_keys[right_idx], right_values[right_idx]);
      ++right_idx;
    }
    hash = 1469598103934665603ULL;
    for (const auto& kv : out) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  return hash;
}