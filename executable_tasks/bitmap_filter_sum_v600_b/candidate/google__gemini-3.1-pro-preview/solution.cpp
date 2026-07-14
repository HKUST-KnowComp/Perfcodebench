#include "interface.h"
#include <vector>
#include <cstring>
#include <algorithm>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (key_space == 0) return 0;

    // Pre-allocate bitmap to avoid repeated allocations.
    // Size is (key_space + 63) / 64 uint64_t elements.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 63) >> 6;
    std::vector<uint64_t> bitmap(bitmap_size, 0);
    
    uint64_t final_sum = 0;
    const size_t num_probes = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();
    uint64_t* b_ptr = bitmap.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap efficiently.
        // If enabled_keys is very small relative to key_space, we could clear only set bits,
        // but memset is generally extremely fast for typical key_space sizes.
        std::fill(bitmap.begin(), bitmap.end(), 0);

        // Populate bitmap
        for (uint32_t key : enabled_keys) {
            if (key < key_space) {
                b_ptr[key >> 6] |= (1ULL << (key & 63));
            }
        }

        uint64_t current_sum = 0;
        // Hot loop: filter and aggregate
        // We use a pointer-based loop to assist the compiler with vectorization/optimization.
        for (size_t i = 0; i < num_probes; ++i) {
            uint32_t k = p_keys[i];
            // Check if bit is set in bitmap
            if ((b_ptr[k >> 6] >> (k & 63)) & 1) {
                current_sum += p_vals[i];
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}