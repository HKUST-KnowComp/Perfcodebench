#include "interface.h"
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters)
{
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();

    const size_t ln = left_keys.size();
    const size_t rn = right_keys.size();

    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t h = 1469598103934665603ULL;

        while (i < ln && j < rn) {
            if (lk[i] < rk[j]) {
                h = mix(h, lk[i]);
                h = mix(h, lv[i]);
                ++i;
            } else if (lk[i] > rk[j]) {
                h = mix(h, rk[j]);
                h = mix(h, rv[j]);
                ++j;
            } else {
                // duplicate key: keep right value, skip left
                h = mix(h, rk[j]);
                h = mix(h, rv[j]);
                ++i;
                ++j;
            }
        }

        while (i < ln) {
            h = mix(h, lk[i]);
            h = mix(h, lv[i]);
            ++i;
        }

        while (j < rn) {
            h = mix(h, rk[j]);
            h = mix(h, rv[j]);
            ++j;
        }

        hash = h;
    }

    return hash;
}