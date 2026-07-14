#include "interface.h"

#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || key_space == 0) return 0;
    std::vector<uint32_t> last_seen(key_space, 0);
    uint32_t current_gen = 0;
    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        current_gen++;
        uint64_t count = 0;
        for (uint32_t key : keys) {
            if (last_seen[key] != current_gen) {
                count++;
                last_seen[key] = current_gen;
            }
        }
        final_count = count;
    }
    return final_count;
}