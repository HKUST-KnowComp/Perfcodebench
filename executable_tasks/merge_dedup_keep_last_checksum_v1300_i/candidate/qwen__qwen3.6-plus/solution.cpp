#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    (void)iters;

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    size_t i = 0, j = 0;
    const size_t L = left_keys.size();
    const size_t R = right_keys.size();

    while (i < L || j < R) {
        uint32_t key;
        uint32_t val;

        if (j == R || (i < L && left_keys[i] < right_keys[j])) {
            key = left_keys[i];
            val = left_values[i];
            while (++i < L && left_keys[i] == key) {
                val = left_values[i];
            }
        } else if (i == L || left_keys[i] > right_keys[j]) {
            key = right_keys[j];
            val = right_values[j];
            while (++j < R && right_keys[j] == key) {
                val = right_values[j];
            }
        } else {
            key = left_keys[i];
            while (i < L && left_keys[i] == key) {
                ++i;
            }
            val = right_values[j];
            while (++j < R && right_keys[j] == key) {
                val = right_values[j];
            }
        }

        hash ^= key;
        hash *= FNV_PRIME;
        hash ^= val;
        hash *= FNV_PRIME;
    }

    return hash;
}
