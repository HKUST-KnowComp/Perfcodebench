#include "interface.h"
#include <vector>
#include <algorithm>
#include <map>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (domain == 0) return 0;

    // Pre-allocate to avoid reallocations
    std::vector<std::pair<uint32_t, uint64_t>> sorted_updates;
    sorted_updates.reserve(indices.size());

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // 1. Accumulate values using a sorted vector approach for better cache locality and speed
        // instead of unordered_map, which is slow for this specific pattern.
        std::vector<std::pair<uint32_t, uint64_t>> temp;
        temp.reserve(indices.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            temp.push_back({indices[i], static_cast<uint64_t>(values[i])});
        }
        
        std::sort(temp.begin(), temp.end());

        // 2. Compress duplicates (the actual accumulation)
        sorted_updates.clear();
        if (!temp.empty()) {
            uint32_t curr_idx = temp[0].first;
            uint64_t curr_val = temp[0].second;
            for (size_t i = 1; i < temp.size(); ++i) {
                if (temp[i].first == curr_idx) {
                    curr_val += temp[i].second;
                } else {
                    sorted_updates.push_back({curr_idx, curr_val});
                    curr_idx = temp[i].first;
                    curr_val = temp[i].second;
                }
            }
            sorted_updates.push_back({curr_idx, curr_val});
        }

        // 3. Reconstruct the hash.
        // The original code: for (key = 0; key < domain; ++key) hash = mix(hash, accum[key])
        // We must account for the 0 values in the gaps.
        uint64_t hash = 1469598103934665603ULL;
        uint32_t current_key = 0;

        for (const auto& pair : sorted_updates) {
            uint32_t key = pair.first;
            uint64_t val = pair.second;

            // Process the zeros between current_key and key
            // Note: mix(hash, 0) is a specific operation. 
            // We can't easily skip it, but we can optimize if we realize 
            // that multiple mix(hash, 0) calls can be pre-calculated or handled.
            // However, for correctness, we must apply it.
            for (uint32_t k = current_key; k < key; ++k) {
                hash = mix(hash, 0ULL);
            }

            // Process the non-zero value
            hash = mix(hash, val);
            current_key = key + 1;
        }

        // Process remaining zeros up to domain
        for (uint32_t k = current_key; k < domain; ++k) {
            hash = mix(hash, 0ULL);
        }
        
        final_hash = hash;
    }

    return final_hash;
}