#include "interface.h"

#include <unordered_set>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    if (iters <= 0) return 0;
    std::unordered_set<uint32_t> seen;
    for (uint32_t key : keys) {
        seen.insert(key);
    }
    return static_cast<uint64_t>(seen.size());
}
