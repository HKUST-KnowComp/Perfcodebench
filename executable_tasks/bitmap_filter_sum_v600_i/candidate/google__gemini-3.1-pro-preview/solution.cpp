#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

/**
 * Optimized bitmap-based filter aggregation.
 * 
 * Performance improvements:
 * 1. Replaced std::unordered_set with a bitset (vector of uint64_t) for O(1) lookups with low constant factors.
 * 2. Moved the bitmap construction outside the 'iters' loop, as the enabled_keys are invariant.
 * 3. Used a compact bit representation to improve cache efficiency and reduce memory footprint.
 * 4. Ensured the summation logic matches the baseline's behavior (returning the sum of the last iteration).
 */
uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (iters <= 0) return 0;

    // Allocate bitmap. key_space is the upper bound for keys.
    // Using uint64_t blocks for efficient bit manipulation.
    size_t num_blocks = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(num_blocks, 0);

    // Populate the bitmap once outside the loop.
    for (uint32_t key : enabled_keys) {
        if (key < key_space) {
            bitmap[key >> 6] |= (1ULL << (key & 63));
        }
    }

    uint64_t final_sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();
    const uint64_t* p_bitmap = bitmap.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        // The core hot loop: check bitmap and accumulate values.
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = p_keys[i];
            // Check if key is within bounds and bit is set.
            // Note: key_space check is technically required if probe_keys can exceed key_space.
            if (key < key_space) {
                if ((p_bitmap[key >> 6] >> (key & 63)) & 1ULL) {
                    current_sum += static_cast<uint64_t>(p_vals[i]);
                }
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}
