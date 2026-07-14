#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0 || keys.empty()) return 0;
    
    // Generation array avoids O(key_space) clearing per iteration.
    // Stores the 1-based iteration number when a key was last seen.
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        uint32_t gen = static_cast<uint32_t>(iter) + 1;
        for (uint32_t k : keys) {
            if (seen[k] != gen) {
                seen[k] = gen;
                ++count;
            }
        }
    }
    return count;
}