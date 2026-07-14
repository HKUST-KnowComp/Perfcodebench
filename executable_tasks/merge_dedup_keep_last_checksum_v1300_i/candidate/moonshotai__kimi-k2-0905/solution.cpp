#include "interface.h"
#include <cstdint>

static inline uint64_t mix(uint64_t h, uint32_t v) {
    h ^= static_cast<uint64_t>(v);
    h *= 1099511628211ULL;
    return h;
}

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters)
{
    const uint32_t *lk = left_keys.data();
    const uint32_t *lv = left_values.data();
    const uint32_t *rk = right_keys.data();
    const uint32_t *rv = right_values.data();

    const size_t nl = left_keys.size();
    const size_t nr = right_keys.size();

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        hash = 1469598103934665603ULL;

        while (i < nl && j < nr) {
            const uint32_t lkey = lk[i];
            const uint32_t rkey = rk[j];
            if (lkey < rkey) {
                hash = mix(hash, lkey);
                hash = mix(hash, lv[i]);
                ++i;
            } else if (lkey > rkey) {
                hash = mix(hash, rkey);
                hash = mix(hash, rv[j]);
                ++j;
            } else {
                hash = mix(hash, rkey);
                hash = mix(hash, rv[j]);
                ++i; ++j;
            }
        }
        while (i < nl) {
            hash = mix(hash, lk[i]);
            hash = mix(hash, lv[i]);
            ++i;
        }
        while (j < nr) {
            hash = mix(hash, rk[j]);
            hash = mix(hash, rv[j]);
            ++j;
        }
    }
    return hash;
}