#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters)
{
    const uint32_t *lk = left_keys.data(),  *rk = right_keys.data();
    const uint32_t *lv = left_values.data(), *rv = right_values.data();
    size_t i = 0, j = 0;
    const size_t n = left_keys.size(), m = right_keys.size();

    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        i = 0; j = 0;
        uint64_t h = 1469598103934665603ULL;

        while (i < n && j < m) {
            if (lk[i] < rk[j]) {
                h = mix(h, lk[i]);
                h = mix(h, lv[i]);
                ++i;
            } else if (lk[i] > rk[j]) {
                h = mix(h, rk[j]);
                h = mix(h, rv[j]);
                ++j;
            } else { // equal keys: keep right side
                h = mix(h, rk[j]);
                h = mix(h, rv[j]);
                ++i; ++j;
            }
        }
        while (i < n) {
            h = mix(h, lk[i]);
            h = mix(h, lv[i]);
            ++i;
        }
        while (j < m) {
            h = mix(h, rk[j]);
            h = mix(h, rv[j]);
            ++j;
        }
        hash = h;
    }
    return hash;
}