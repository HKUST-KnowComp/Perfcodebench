#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    (void)iters;
    if (keys.empty()) return 0;
    std::vector<uint32_t> tmp(keys);
    std::sort(tmp.begin(), tmp.end());
    auto last = std::unique(tmp.begin(), tmp.end());
    return static_cast<uint64_t>(last - tmp.begin());
}