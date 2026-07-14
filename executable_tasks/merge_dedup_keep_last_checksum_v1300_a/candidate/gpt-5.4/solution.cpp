#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct KV {
  uint32_t key;
  uint32_t value;
};

static inline void dedup_keep_last_sorted(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    std::vector<KV>& out) {
  out.clear();
  const std::size_t n = keys.size();
  out.reserve(n);
  if (n == 0) return;

  uint32_t cur_key = keys[0];
  uint32_t cur_value = values[0];
  for (std::size_t i = 1; i < n; ++i) {
    const uint32_t k = keys[i];
    const uint32_t v = values[i];
    if (k == cur_key) {
      cur_value = v;
    } else {
      out.push_back({cur_key, cur_value});
      cur_key = k;
      cur_value = v;
    }
  }
  out.push_back({cur_key, cur_value});
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  std::vector<KV> left_dedup;
  std::vector<KV> right_dedup;
  dedup_keep_last_sorted(left_keys, left_values, left_dedup);
  dedup_keep_last_sorted(right_keys, right_values, right_dedup);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    std::size_t i = 0;
    std::size_t j = 0;
    const std::size_t ln = left_dedup.size();
    const std::size_t rn = right_dedup.size();

    while (i < ln && j < rn) {
      const uint32_t lk = left_dedup[i].key;
      const uint32_t rk = right_dedup[j].key;
      if (lk < rk) {
        h = mix(h, lk);
        h = mix(h, left_dedup[i].value);
        ++i;
      } else if (rk < lk) {
        h = mix(h, rk);
        h = mix(h, right_dedup[j].value);
        ++j;
      } else {
        h = mix(h, rk);
        h = mix(h, right_dedup[j].value);
        ++i;
        ++j;
      }
    }

    while (i < ln) {
      h = mix(h, left_dedup[i].key);
      h = mix(h, left_dedup[i].value);
      ++i;
    }
    while (j < rn) {
      h = mix(h, right_dedup[j].key);
      h = mix(h, right_dedup[j].value);
      ++j;
    }

    hash = h;
  }
  return hash;
}
