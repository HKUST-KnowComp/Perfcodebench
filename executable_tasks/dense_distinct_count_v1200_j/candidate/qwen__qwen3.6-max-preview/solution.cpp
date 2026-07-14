#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    const uint32_t* k_data = keys.data();
    size_t k_size = keys.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        uint32_t tag = static_cast<uint32_t>(iter) + 1;
        for (size_t i = 0; i < k_size; ++i) {
            uint32_t key = k_data[i];
            if (seen[key] != tag) {
                seen[key] = tag;
                ++count;
            }
        }
    }
    return count;
}