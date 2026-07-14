#include "interface.h"
#include <unordered_set>
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t MAX_KEY_SPACE_FOR_BITSET = 10000000;
    if (key_space <= MAX_KEY_SPACE_FOR_BITSET) {
        std::vector<int32_t> visited(key_space, -1);
        uint64_t count = 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t local_count = 0;
            int32_t tag = static_cast<int32_t>(iter);
            for (uint32_t key : keys) {
                if (visited[key] != tag) {
                    visited[key] = tag;
                    ++local_count;
                }
            }
            count = local_count;
        }
        return count;
    } else {
        std::unordered_set<uint32_t> seen;
        uint64_t count = 0;
        for (int iter = 0; iter < iters; ++iter) {
            seen.clear();
            for (uint32_t key : keys) {
                seen.insert(key);
            }
            count = static_cast<uint64_t>(seen.size());
        }
        return count;
    }
}