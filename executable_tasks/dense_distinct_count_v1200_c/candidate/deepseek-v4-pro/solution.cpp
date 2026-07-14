#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)iters;
    std::vector<uint8_t> seen(key_space, 0);
    uint64_t count = 0;
    for (uint32_t key : keys) {
        if (seen[key] == 0) {
            seen[key] = 1;
            ++count;
        }
    }
    return count;
}