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
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(left_keys.size() + right_keys.size());
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    merged.clear();
    auto left_it = left_keys.begin();
    auto left_end = left_keys.end();
    auto right_it = right_keys.begin();
    auto right_end = right_keys.end();
    auto left_val_it = left_values.begin();
    auto right_val_it = right_values.begin();
    while (left_it != left_end && right_it != right_end) {
      if (*left_it < *right_it) {
        merged.emplace_back(*left_it, *left_val_it);
        ++left_it;
        ++left_val_it;
      } else if (*right_it < *left_it) {
        merged.emplace_back(*right_it, *right_val_it);
        ++right_it;
        ++right_val_it;
      } else {
        merged.emplace_back(*right_it, *right_val_it);
        ++left_it;
        ++left_val_it;
        ++right_it;
        ++right_val_it;
      }
    }
    while (left_it != left_end) {
      merged.emplace_back(*left_it, *left_val_it);
      ++left_it;
      ++left_val_it;
    }
    while (right_it != right_end) {
      merged.emplace_back(*right_it, *right_val_it);
      ++right_it;
      ++right_val_it;
    }
    hash = 1469598103934665603ULL;
    uint32_t last_key = 0;
    uint32_t last_value = 0;
    bool is_first = true;
    for (const auto& kv : merged) {
      if (is_first || kv.first != last_key) {
        if (!is_first) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_value);
        }
        last_key = kv.first;
        last_value = kv.second;
        is_first = false;
      } else {
        last_value = kv.second;
      }
    }
    if (!is_first) {
      hash = mix(hash, last_key);
      hash = mix(hash, last_value);
    }
  }
  return hash;
}