#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || iters <= 0) return 0;
    if (key_space == 0) return 0;

    const uint32_t DIRECT_THRESHOLD = 10000000;
    
    if (key_space <= DIRECT_THRESHOLD) {
        std::vector<uint32_t> seen(key_space, 0);
        uint32_t gen = 0;
        uint64_t count = 0;
        
        for (int iter = 0; iter < iters; ++iter) {
            gen++;
            uint64_t cur_count = 0;
            for (uint32_t k : keys) {
                if (seen[k] != gen) {
                    seen[k] = gen;
                    cur_count++;
                }
            }
            count = cur_count;
        }
        return count;
    } else {
        std::vector<uint32_t> sorted_keys = keys;
        uint64_t count = 0;
        for (int iter = 0; iter < iters; ++iter) {
            std::sort(sorted_keys.begin(), sorted_keys.end());
            auto last = std::unique(sorted_keys.begin(), sorted_keys.end());
            count = static_cast<uint64_t>(last - sorted_keys.begin());
        }
        return count;
    }
}