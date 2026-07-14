#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || iters <= 0) {
        return 0;
    }

    // Threshold for using direct addressing table vs sorting
    // 1MB is a safe threshold to fit in L3 cache and avoid OOM on large key_space
    const uint32_t CACHE_THRESHOLD = 1000000;
    
    uint64_t count = 0;

    if (key_space <= CACHE_THRESHOLD) {
        // Use direct addressing table for small key_space
        std::vector<uint8_t> seen(key_space, 0);
        
        for (int iter = 0; iter < iters; ++iter) {
            // Fast reset using memset
            std::memset(seen.data(), 0, key_space);
            count = 0;
            
            for (uint32_t key : keys) {
                if (key < key_space && !seen[key]) {
                    seen[key] = 1;
                    ++count;
                }
            }
        }
    } else {
        // Use sorting for large key_space
        std::vector<uint32_t> sorted_keys;
        sorted_keys.reserve(keys.size());
        
        for (int iter = 0; iter < iters; ++iter) {
            // Copy and sort
            sorted_keys = keys;
            std::sort(sorted_keys.begin(), sorted_keys.end());
            
            // Count unique
            auto last = std::unique(sorted_keys.begin(), sorted_keys.end());
            count = static_cast<uint64_t>(last - sorted_keys.begin());
        }
    }

    return count;
}