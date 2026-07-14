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
    
    if (probe_keys.empty()) return 0;

    // Pre-allocate bitmap buffer outside the iteration loop to avoid repeated allocations.
    // Using a bit-packed uint64_t array for cache efficiency and fast lookups.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 64) / 64;
    std::vector<uint64_t> bitmap(bitmap_size, 0);
    
    uint64_t total_sum = 0;
    const size_t n = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap for each iteration as per baseline logic
        std::memset(bitmap.data(), 0, bitmap.size() * sizeof(uint64_t));
        
        for (uint32_t key : enabled_keys) {
            if (key <= key_space) {
                bitmap[key >> 6] |= (1ULL << (key & 63));
            }
        }

        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Unroll the loop to improve throughput
        for (; i + 4 <= n; i += 4) {
            uint32_t k0 = probe_keys[i];
            uint32_t k1 = probe_keys[i+1];
            uint32_t k2 = probe_keys[i+2];
            uint32_t k3 = probe_keys[i+3];

            if ((bitmap[k0 >> 6] >> (k0 & 63)) & 1) current_sum += values[i];
            if ((bitmap[k1 >> 6] >> (k1 & 63)) & 1) current_sum += values[i+1];
            if ((bitmap[k2 >> 6] >> (k2 & 63)) & 1) current_sum += values[i+2];
            if ((bitmap[k3 >> 6] >> (k3 & 63)) & 1) current_sum += values[i+3];
        }

        // Handle remaining elements
        for (; i < n; ++i) {
            uint32_t k = probe_keys[i];
            if ((bitmap[k >> 6] >> (k & 63)) & 1) {
                current_sum += values[i];
            }
        }
        
        total_sum = current_sum;
    }

    return total_sum;
}