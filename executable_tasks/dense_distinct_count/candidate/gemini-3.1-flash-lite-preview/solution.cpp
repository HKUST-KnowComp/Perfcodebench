#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    
    // Use a vector of bytes as a boolean flag array for O(1) access.
    // This is significantly faster than std::unordered_set for dense/bounded keys.
    std::vector<uint8_t> seen(key_space, 0);
    std::vector<uint32_t> touched;
    touched.reserve(keys.size());
    
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (uint32_t key : keys) {
            if (key < key_space && seen[key] == 0) {
                seen[key] = 1;
                touched.push_back(key);
                count++;
            }
        }
        
        // Reset only the entries we modified to keep the next iteration fast
        for (uint32_t key : touched) {
            seen[key] = 0;
        }
        touched.clear();
    }
    return count;
}