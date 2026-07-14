#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> visited(key_space);
    uint64_t epoch = 1;
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_count = 0;
        for (uint32_t key : keys) {
            if (visited[key] != epoch) {
                visited[key] = static_cast<uint32_t>(epoch);
                ++local_count;
            }
        }
        count = local_count;
        ++epoch;
        if (epoch == 0) {
            std::fill(visited.begin(), visited.end(), 0);
            epoch = 1;
        }
    }
    return count;
}