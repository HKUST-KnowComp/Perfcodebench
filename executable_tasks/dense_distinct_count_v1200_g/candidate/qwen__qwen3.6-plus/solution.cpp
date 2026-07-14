#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0 || keys.empty()) return 0;
    
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    uint32_t gen = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != gen) {
                seen[key] = gen;
                ++current_count;
            }
        }
        count = current_count;
        ++gen;
    }
    return count;
}