#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    (void)iters; // not needed, compute once
    if (keys.empty()) return 0;
    std::vector<uint32_t> sorted(keys);
    std::sort(sorted.begin(), sorted.end());
    auto last = std::unique(sorted.begin(), sorted.end());
    return static_cast<uint64_t>(std::distance(sorted.begin(), last));
}