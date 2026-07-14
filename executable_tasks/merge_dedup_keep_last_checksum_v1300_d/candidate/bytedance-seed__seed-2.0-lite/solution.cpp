#include "interface.h"

#include <vector>
#include <utility>

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
  // Pre-deduplicate inputs once to retain only last occurrence of each key, matching baseline semantics
  std::vector<std::pair<uint32_t, uint32_t>> dedup_left;
  dedup_left.reserve(left_keys.size());
  for (size_t i = 0; i < left_keys.size(); ++i) {
    const uint32_t k = left_keys[i];
    const uint32_t v = left_values[i];
    if (!dedup_left.empty() && dedup_left.back().first == k) {
      dedup_left.back().second = v;
    } else {
      dedup_left.emplace_back(k, v);
    }
  }

  std::vector<std::pair<uint32_t, uint32_t>> dedup_right;
  dedup_right.reserve(right_keys.size());
  for (size_t i = 0; i < right_keys.size(); ++i) {
    const uint32_t k = right_keys[i];
    const uint32_t v = right_values[i];
    if (!dedup_right.empty() && dedup_right.back().first == k) {
      dedup_right.back().second = v;
    } else {
      dedup_right.emplace_back(k, v);
    }
  }

  uint64_t final_hash = 0;
  const size_t n = dedup_left.size();
  const size_t m = dedup_right.size();

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t hash = 1469598103934665603ULL;

    // Linear two-pointer merge of pre-sorted deduped runs
    while (i < n && j < m) {
      const uint32_t lk = dedup_left[i].first;
      const uint32_t lv = dedup_left[i].second;
      const uint32_t rk = dedup_right[j].first;
      const uint32_t rv = dedup_right[j].second;

      if (lk < rk) {
        hash = mix(hash, lk);
        hash = mix(hash, lv);
        ++i;
      } else if (rk < lk) {
        hash = mix(hash, rk);
        hash = mix(hash, rv);
        ++j;
      } else {
        // Duplicate keys: keep right-side value per requirements
        hash = mix(hash, rk);
        hash = mix(hash, rv);
        ++i;
        ++j;
      }
    }

    // Flush remaining entries from left run
    while (i < n) {
      hash = mix(hash, dedup_left[i].first);
      hash = mix(hash, dedup_left[i].second);
      ++i;
    }
    // Flush remaining entries from right run
    while (j < m) {
      hash = mix(hash, dedup_right[j].first);
      hash = mix(hash, dedup_right[j].second);
      ++j;
    }

    final_hash = hash;
  }

  return final_hash;
}