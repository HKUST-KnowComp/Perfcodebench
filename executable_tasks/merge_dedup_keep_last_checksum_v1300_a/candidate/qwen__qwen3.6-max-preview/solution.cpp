#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    uint64_t hash = 0;
    const size_t n = left_keys.size();
    const size_t m = right_keys.size();
    const uint32_t* lk_ptr = left_keys.data();
    const uint32_t* lv_ptr = left_values.data();
    const uint32_t* rk_ptr = right_keys.data();
    const uint32_t* rv_ptr = right_values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        size_t i = 0, j = 0;
        bool has_last = false;
        uint32_t last_k = 0, last_v = 0;

        auto flush_last = [&]() {
            if (has_last) {
                hash ^= last_k; hash *= 1099511628211ULL;
                hash ^= last_v; hash *= 1099511628211ULL;
            }
        };

        auto consider = [&](uint32_t k, uint32_t v) {
            if (has_last && k == last_k) {
                last_v = v;
            } else {
                flush_last();
                last_k = k; last_v = v; has_last = true;
            }
        };

        while (i < n && j < m) {
            uint32_t lk = lk_ptr[i];
            uint32_t rk = rk_ptr[j];
            if (lk < rk) {
                consider(lk, lv_ptr[i]);
                ++i;
            } else if (lk > rk) {
                consider(rk, rv_ptr[j]);
                ++j;
            } else {
                consider(lk, lv_ptr[i]);
                consider(rk, rv_ptr[j]);
                ++i; ++j;
            }
        }
        while (i < n) { consider(lk_ptr[i], lv_ptr[i]); ++i; }
        while (j < m) { consider(rk_ptr[j], rv_ptr[j]); ++j; }
        flush_last();
    }
    return hash;
}