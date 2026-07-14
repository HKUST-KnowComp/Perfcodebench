#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    std::vector<uint32_t> buf(keys.size());
    uint64_t count = 0;
    for (int i = 0; i < iters; ++i) {
        std::copy(keys.begin(), keys.end(), buf.begin());
        std::sort(buf.begin(), buf.end());
        count = std::unique(buf.begin(), buf.end()) - buf.begin();
    }
    return count;
}