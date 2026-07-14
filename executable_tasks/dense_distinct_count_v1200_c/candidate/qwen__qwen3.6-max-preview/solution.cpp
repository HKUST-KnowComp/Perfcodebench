#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    uint32_t* seen_ptr = seen.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        uint32_t gen = static_cast<uint32_t>(iter) + 1;
        for (uint32_t key : keys) {
            if (seen_ptr[key] != gen) {
                seen_ptr[key] = gen;
                ++count;
            }
        }
    }
    return count;
}