#include "interface.h"
#include <vector>
#include <cstring>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (iters <= 0) return 0;

    // Use a bitset for O(1) lookup with minimal memory footprint.
    // key_space is the upper bound for keys.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 7) / 8;
    std::vector<uint8_t> bitmap(bitmap_size, 0);
    
    uint64_t final_sum = 0;
    const size_t num_probes = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap for each iteration as per baseline logic
        std::memset(bitmap.data(), 0, bitmap.size());
        
        // Populate bitmap
        for (uint32_t key : enabled_keys) {
            if (key < key_space) {
                bitmap[key >> 3] |= (1 << (key & 7));
            }
        }

        uint64_t current_sum = 0;
        // Probe keys and accumulate values
        // Unrolling or SIMD could be applied here, but standard bit-check is already very fast.
        for (size_t i = 0; i < num_probes; ++i) {
            uint32_t pk = p_keys[i];
            if (pk < key_space) {
                if (bitmap[pk >> 3] & (1 << (pk & 7))) {
                    current_sum += static_cast<uint64_t>(p_vals[i]);
                }
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}