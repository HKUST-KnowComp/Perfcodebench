#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (keys.empty()) return 0;

    // Find the range of keys to build a direct lookup table.
    // Since keys are sorted, we can find the max key to size our table.
    uint32_t max_key = keys.back();
    
    // Create a lookup table for O(1) access.
    // We initialize with a value that indicates 'not found' or handle via binary search if out of bounds.
    // However, the problem implies probes will be valid keys or we must match lower_bound behavior.
    // To perfectly match std::lower_bound(keys.begin(), keys.end(), probe), 
    // we need to map every possible probe value to the index of the first element >= probe.
    
    // Optimization: Pre-calculate the mapping for all possible probe values.
    // Since we don't know the range of probes, but we know keys are sorted,
    // we can build a lookup table up to max_key.
    
    std::vector<uint32_t> lookup(max_key + 1);
    size_t current_key_idx = 0;
    for (uint32_t i = 0; i <= max_key; ++i) {
        while (current_key_idx + 1 < keys.size() && keys[current_key_idx + 1] <= i) {
            current_key_idx++;
        }
        // If i is >= keys[current_key_idx], the lower_bound index is current_key_idx.
        // But we need to handle the case where i < keys[0].
        if (i < keys[0]) {
            lookup[i] = 0;
        } else {
            // Find the first key >= i. 
            // Actually, the baseline uses std::lower_bound which returns the first element NOT LESS than probe.
            // If probe is 5 and keys are [2, 4, 6], lower_bound returns index 2 (value 6).
            // If probe is 7, lower_bound returns index 3 (end).
            // The baseline code: const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
            // This will crash if it == keys.end() and we access values[idx]. 
            // Assuming probes are always within valid range of keys for the task.
            
            // Let's re-implement the lookup to match lower_bound exactly.
            // We need to find the smallest index j such that keys[j] >= i.
        }
    }

    // Re-evaluating: The most robust way to optimize without knowing probe range is to 
    // use the fact that keys are sorted. If probes are also somewhat sorted or repeated, 
    // we can optimize. But the most reliable high-speed way is a direct map if max_key is reasonable.
    // If max_key is too large, we use the binary search but optimize the loop.
    
    // Let's use a hybrid approach: if max_key is small, use lookup. If large, use binary search.
    // Given the task 'dense_id_remap', max_key is likely manageable.
    
    if (max_key < 10000000) {
        std::vector<uint32_t> val_lookup(max_key + 1);
        size_t k_idx = 0;
        for (uint32_t i = 0; i <= max_key; ++i) {
            while (k_idx < keys.size() && keys[k_idx] < i) {
                k_idx++;
            }
            // k_idx is now the index of the first key >= i
            // If k_idx == keys.size(), it's the end. 
            // The baseline code: values[idx] where idx = it - keys.begin().
            // If it == keys.end(), idx == keys.size(). This would be out of bounds.
            // We assume probes are valid such that it != keys.end().
            val_lookup[i] = values[k_idx];
        }

        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
            for (uint32_t probe : probes) {
                hash = mix(hash, val_lookup[probe]);
            }
        }
        return hash;
    } else {
        // Fallback to optimized binary search
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
            for (uint32_t probe : probes) {
                auto it = std::lower_bound(keys.begin(), keys.end(), probe);
                hash = mix(hash, values[std::distance(keys.begin(), it)]);
            }
        }
        return hash;
    }
}