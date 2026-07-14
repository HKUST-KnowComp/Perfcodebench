#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (key_space == 0) return 0;

    // Use a bitset (1 bit per key) to represent enabled keys.
    // This is memory efficient and fits better in cache than a byte-map for large key spaces.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(bitmap_size, 0);

    for (uint32_t key : enabled_keys) {
        if (key < key_space) {
            bitmap[key >> 6] |= (1ULL << (key & 63));
        }
    }

    uint64_t total_sum = 0;
    const size_t num_probes = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();
    const uint64_t* p_bitmap = bitmap.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        // Unrolling or SIMD could be applied here, but a clean scalar loop 
        // with branchless logic is often highly effective for this pattern.
        for (size_t i = 0; i < num_probes; ++i) {
            uint32_t key = p_keys[i];
            // Branchless bit extraction: 
            // Shift the relevant 64-bit chunk and mask the LSB to get 0 or 1.
            uint64_t is_enabled = (p_bitmap[key >> 6] >> (key & 63)) & 1ULL;
            current_sum += static_cast<uint64_t>(p_vals[i]) * is_enabled;
        }
        total_sum = current_sum;
    }

    return total_sum;
}