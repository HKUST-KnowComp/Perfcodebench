#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> last_seen(key_space, 0);
    uint32_t marker = 0;
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        ++marker;
        for (uint32_t key : keys) {
            if (last_seen[key] != marker) {
                last_seen[key] = marker;
                ++count;
            }
        }
    }
    return count;
}