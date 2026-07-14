#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    uint64_t hash = 0;
    size_t n = left_keys.size();
    size_t m = right_keys.size();
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        size_t i = 0, j = 0;
        uint32_t cur_k = 0, cur_v = 0;
        bool has_cur = false;

        while (i < n && j < m) {
            uint32_t k, v;
            if (lk[i] <= rk[j]) { k = lk[i]; v = lv[i]; ++i; }
            else { k = rk[j]; v = rv[j]; ++j; }
            
            if (has_cur) {
                if (k == cur_k) { cur_v = v; }
                else {
                    hash ^= cur_k; hash *= 1099511628211ULL;
                    hash ^= cur_v; hash *= 1099511628211ULL;
                    cur_k = k; cur_v = v;
                }
            } else {
                cur_k = k; cur_v = v; has_cur = true;
            }
        }
        while (i < n) {
            uint32_t k = lk[i], v = lv[i]; ++i;
            if (has_cur) {
                if (k == cur_k) { cur_v = v; }
                else {
                    hash ^= cur_k; hash *= 1099511628211ULL;
                    hash ^= cur_v; hash *= 1099511628211ULL;
                    cur_k = k; cur_v = v;
                }
            } else {
                cur_k = k; cur_v = v; has_cur = true;
            }
        }
        while (j < m) {
            uint32_t k = rk[j], v = rv[j]; ++j;
            if (has_cur) {
                if (k == cur_k) { cur_v = v; }
                else {
                    hash ^= cur_k; hash *= 1099511628211ULL;
                    hash ^= cur_v; hash *= 1099511628211ULL;
                    cur_k = k; cur_v = v;
                }
            } else {
                cur_k = k; cur_v = v; has_cur = true;
            }
        }
        if (has_cur) {
            hash ^= cur_k; hash *= 1099511628211ULL;
            hash ^= cur_v; hash *= 1099511628211ULL;
        }
    }
    return hash;
}