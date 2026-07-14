#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Allocate a direct-index array one larger than the maximum key to safely hold all keys.
    std::vector<uint32_t> seen(static_cast<size_t>(key_space) + 1, 0);
    uint64_t count = 0;
    uint32_t iter_id = 0;
    for (int iter = 0; iter < iters; ++iter) {
        ++iter_id;
        uint64_t local_count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != iter_id) {
                seen[key] = iter_id;
                ++local_count;
            }
        }
        count = local_count;
    }
    return count;
}