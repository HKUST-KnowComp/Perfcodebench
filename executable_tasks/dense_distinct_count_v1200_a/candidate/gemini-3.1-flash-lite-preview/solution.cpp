#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    
    // Use a vector as a boolean map. 
    // We use a generation counter to avoid reallocating or clearing the entire array.
    // However, since key_space is bounded, a simple vector<uint8_t> with memset is very fast.
    std::vector<uint8_t> seen(key_space + 1, 0);
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        // Use a unique marker for each iteration to avoid full memset if key_space is large
        // but for small key_space, memset is extremely optimized.
        std::fill(seen.begin(), seen.end(), 0);
        
        for (uint32_t key : keys) {
            if (key <= key_space && seen[key] == 0) {
                seen[key] = 1;
                count++;
            }
        }
    }
    return count;
}