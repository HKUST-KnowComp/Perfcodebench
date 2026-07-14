#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    std::size_t n_left = left_keys.size();
    std::size_t n_right = right_keys.size();

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        std::size_t i = 0, j = 0;
        uint32_t cur_key = 0, cur_val = 0;
        bool has_cur = false;

        while (i < n_left && j < n_right) {
            uint32_t k, v;
            if (lk[i] < rk[j]) {
                k = lk[i]; v = lv[i]; ++i;
            } else if (lk[i] > rk[j]) {
                k = rk[j]; v = rv[j]; ++j;
            } else {
                k = rk[j]; v = rv[j]; ++i; ++j;
            }

            if (has_cur && cur_key != k) {
                hash ^= cur_key; hash *= FNV_PRIME;
                hash ^= cur_val; hash *= FNV_PRIME;
            }
            cur_key = k; cur_val = v; has_cur = true;
        }

        while (i < n_left) {
            uint32_t k = lk[i], v = lv[i]; ++i;
            if (has_cur && cur_key != k) {
                hash ^= cur_key; hash *= FNV_PRIME;
                hash ^= cur_val; hash *= FNV_PRIME;
            }
            cur_key = k; cur_val = v; has_cur = true;
        }

        while (j < n_right) {
            uint32_t k = rk[j], v = rv[j]; ++j;
            if (has_cur && cur_key != k) {
                hash ^= cur_key; hash *= FNV_PRIME;
                hash ^= cur_val; hash *= FNV_PRIME;
            }
            cur_key = k; cur_val = v; has_cur = true;
        }

        if (has_cur) {
            hash ^= cur_key; hash *= FNV_PRIME;
            hash ^= cur_val; hash *= FNV_PRIME;
        }
    }
    return hash;
}