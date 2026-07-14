#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    std::vector<uint32_t> buf(keys);
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::sort(buf.begin(), buf.end());
        uint64_t c = 0;
        if (!buf.empty()) {
            c = 1;
            for (size_t i = 1; i < buf.size(); ++i) {
                if (buf[i] != buf[i-1]) ++c;
            }
        }
        count = c;
    }
    return count;
}