#include "interface.h"
#include <vector>
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
    int iters) {
    
    const size_t n = left_keys.size();
    const size_t m = right_keys.size();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        size_t i = 0;
        size_t j = 0;
        
        uint32_t last_key = 0;
        uint32_t last_val = 0;
        bool first = true;

        auto emit = [&](uint32_t k, uint32_t v) {
            if (!first && k == last_key) {
                // Keep-last semantics: overwrite the value for the current key
                last_val = v;
            } else {
                if (!first) {
                    // Commit previous key-value pair to hash
                    hash = mix(hash, static_cast<uint64_t>(last_key));
                    hash = mix(hash, static_cast<uint64_t>(last_val));
                }
                last_key = k;
                last_val = v;
                first = false;
            }
        };

        // Two-pointer merge
        while (i < n && j < m) {
            if (left_keys[i] < right_keys[j]) {
                emit(left_keys[i], left_values[i]);
                i++;
            } else if (left_keys[i] > right_keys[j]) {
                emit(right_keys[j], right_values[j]);
                j++;
            } else {
                // Duplicate key: right side takes precedence in keep-last
                // We must emit the right side, but the logic in 'emit' handles the overwrite
                // However, to strictly follow the original logic where 'out' is built from a sorted list
                // and duplicates are replaced, we treat the right key as the 'current' one.
                // Since the original code sorted (left, right) and then iterated, 
                // for equal keys, the right one appeared later in the 'merged' list.
                emit(left_keys[i], left_values[i]);
                emit(right_keys[j], right_values[j]);
                i++;
                j++;
            }
        }

        while (i < n) {
            emit(left_keys[i], left_values[i]);
            i++;
        }
        while (j < m) {
            emit(right_keys[j], right_values[j]);
            j++;
        }

        // Final commit
        if (!first) {
            hash = mix(hash, static_cast<uint64_t>(last_key));
            hash = mix(hash, static_cast<uint64_t>(last_val));
        }
    }

    return hash;
}
