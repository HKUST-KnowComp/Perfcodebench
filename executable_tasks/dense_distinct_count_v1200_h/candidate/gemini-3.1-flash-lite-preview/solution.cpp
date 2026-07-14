#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    
    // Use a byte vector as a boolean flag array to track seen keys.
    // This is significantly faster than std::unordered_set due to cache locality
    // and the elimination of hashing/collision overhead.
    std::vector<uint8_t> seen(key_space, 0);
    std::vector<uint32_t> touched_indices;
    touched_indices.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        touched_indices.clear();
        
        for (uint32_t key : keys) {
            if (key < key_space) {
                if (seen[key] == 0) {
                    seen[key] = 1;
                    touched_indices.push_back(key);
                    count++;
                }
            }
        }
        
        // Reset only the entries we modified to keep the next iteration fast
        for (uint32_t idx : touched_indices) {
            seen[idx] = 0;
        }
    }
    return count;
}