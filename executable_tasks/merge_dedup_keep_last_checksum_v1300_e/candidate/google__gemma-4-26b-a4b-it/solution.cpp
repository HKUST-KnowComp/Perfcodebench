#include "interface.h"

#include <cstdint>
#include <vector>

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
    int iters) {
    
    uint64_t hash = 0;
    const size_t n_left = left_keys.size();
    const size_t n_right = right_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        size_t i = 0;
        size_t j = 0;
        
        uint32_t last_key = 0;
        uint32_t last_val = 0;
        bool first = true;

        auto emit = [&](uint32_t k, uint32_t v) {
            if (!first && k == last_key) {
                last_val = v;
            } else {
                if (!first) {
                    hash = mix(hash, last_key);
                    hash = mix(hash, last_val);
                }
                last_key = k;
                last_val = v;
                first = false;
            }
        };

        while (i < n_left && j < n_right) {
            if (left_keys[i] < right_keys[j]) {
                emit(left_keys[i], left_values[i]);
                i++;
            } else if (right_keys[j] < left_keys[i]) {
                emit(right_keys[j], right_values[j]);
                j++;
            } else {
                // Duplicate key: keep right-side value
                emit(right_keys[j], right_values[j]);
                i++;
                j++;
            }
        }

        while (i < n_left) {
            emit(left_keys[i], left_values[i]);
            i++;
        }
        while (j < n_right) {
            emit(right_keys[j], right_values[j]);
            j++;
        }

        if (!first) {
            hash = mix(hash, last_key);
            hash = mix(hash, last_val);
        }
    }

    return hash;
}