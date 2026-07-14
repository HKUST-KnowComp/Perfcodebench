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
  std::vector<std::pair<uint32_t, uint32_t>> out;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    merged.clear();
    merged.reserve(left_keys.size() + right_keys.size());
    for (std::size_t i = 0; i < left_keys.size(); ++i) {
      merged.emplace_back(left_keys[i], left_values[i]);
    }
    for (std::size_t i = 0; i < right_keys.size(); ++i) {
      merged.emplace_back(right_keys[i], right_values[i]);
    }
    std::stable_sort(
        merged.begin(),
        merged.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    out.clear();
    for (const auto& kv : merged) {
      if (!out.empty() && out.back().first == kv.first) {
        out.back().second = kv.second;
      } else {
        out.push_back(kv);
      }
    }
    hash = 1469598103934665603ULL;
    for (const auto& kv : out) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  return hash;
}
