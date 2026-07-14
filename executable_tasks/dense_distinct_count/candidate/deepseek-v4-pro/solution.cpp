#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    std::vector<uint32_t> gen_array(key_space, 0);
    uint32_t gen = 0;
    uint64_t last_count = 0;
    for (int i = 0; i < iters; ++i) {
        ++gen;
        uint64_t count = 0;
        for (uint32_t k : keys) {
            uint32_t& g = gen_array[k];
            if (g != gen) {
                g = gen;
                ++count;
            }
        }
        last_count = count;
    }
    return last_count;
}