#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a byte vector as a boolean flag array to track seen keys.
    // This avoids hashing overhead and dynamic allocation during the loop.
    std::vector<uint8_t> seen(key_space + 1, 0);
    uint64_t count = 0;
    
    // We use a generation counter to avoid re-allocating or clearing the entire vector.
    // However, for simplicity and performance given the constraints, 
    // std::vector::assign or memset is extremely fast for typical key_space sizes.
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(seen.begin(), seen.end(), 0);
        uint64_t current_count = 0;
        for (uint32_t key : keys) {
            if (key <= key_space && seen[key] == 0) {
                seen[key] = 1;
                current_count++;
            }
        }
        count = current_count;
    }
    return count;
}