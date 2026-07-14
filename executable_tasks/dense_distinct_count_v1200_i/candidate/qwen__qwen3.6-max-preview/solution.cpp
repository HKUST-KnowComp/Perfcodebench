#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || key_space == 0) return 0;
    
    // Direct addressing with generation counter avoids O(key_space) clear per iteration
    // and eliminates hashing/allocation overhead of std::unordered_set.
    std::vector<uint32_t> seen(key_space, 0);
    uint32_t gen = 0;
    uint64_t count = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        ++gen;
        count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != gen) {
                seen[key] = gen;
                ++count;
            }
        }
    }
    return count;
}