#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    
    std::vector<uint32_t> gen(key_space, 0);
    uint64_t count = 0;
    uint32_t current_gen = 1;
    
    const uint32_t* keys_data = keys.data();
    size_t n = keys.size();
    uint32_t* gen_data = gen.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_count = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = keys_data[i];
            if (key < key_space) {
                if (gen_data[key] != current_gen) {
                    gen_data[key] = current_gen;
                    ++local_count;
                }
            }
        }
        count = local_count;
        ++current_gen;
        if (current_gen == 0) {
            std::fill(gen_data, gen_data + key_space, 0);
            current_gen = 1;
        }
    }
    return count;
}
