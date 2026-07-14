#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    const uint32_t* lk_end = lk + left_keys.size();
    const uint32_t* rk_end = rk + right_keys.size();

    hash = 1469598103934665603ULL;

    while (lk < lk_end && rk < rk_end) {
      uint32_t cur_key = (*lk < *rk) ? *lk : *rk;
      uint32_t cur_val = 0;
      while (lk < lk_end && *lk == cur_key) { cur_val = *lv; ++lk; ++lv; }
      while (rk < rk_end && *rk == cur_key) { cur_val = *rv; ++rk; ++rv; }
      hash ^= cur_key;
      hash *= 1099511628211ULL;
      hash ^= cur_val;
      hash *= 1099511628211ULL;
    }
    while (lk < lk_end) {
      uint32_t cur_key = *lk;
      uint32_t cur_val = 0;
      while (lk < lk_end && *lk == cur_key) { cur_val = *lv; ++lk; ++lv; }
      hash ^= cur_key;
      hash *= 1099511628211ULL;
      hash ^= cur_val;
      hash *= 1099511628211ULL;
    }
    while (rk < rk_end) {
      uint32_t cur_key = *rk;
      uint32_t cur_val = 0;
      while (rk < rk_end && *rk == cur_key) { cur_val = *rv; ++rk; ++rv; }
      hash ^= cur_key;
      hash *= 1099511628211ULL;
      hash ^= cur_val;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
