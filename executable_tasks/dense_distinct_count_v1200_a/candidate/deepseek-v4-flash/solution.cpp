#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Timestamp array: for each key, store the last iteration number when seen
    std::vector<uint32_t> seen(key_space, 0);
    uint32_t gen = 0;  // generation counter, never 0 after first increment
    uint64_t distinct = 0;

    for (int iter = 0; iter < iters; ++iter) {
        ++gen;
        if (gen == 0) {  // overflow: reset entire array (rare)
            for (size_t i = 0; i < key_space; ++i) seen[i] = 0;
            gen = 1;
        }
        uint64_t cnt = 0;
        for (uint32_t key : keys) {
            if (seen[key] != gen) {
                seen[key] = gen;
                ++cnt;
            }
        }
        distinct = cnt;  // save count of current iteration
    }
    return distinct;
}
