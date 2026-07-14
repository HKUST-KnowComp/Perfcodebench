#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    std::vector<uint32_t> buf(keys);
    uint64_t count = 0;
    for (int i = 0; i < iters; ++i) {
        std::sort(buf.begin(), buf.end());
        auto last = std::unique(buf.begin(), buf.end());
        count = static_cast<uint64_t>(last - buf.begin());
    }
    return count;
}