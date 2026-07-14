#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters;
    uint64_t sum = 0;
    uint64_t cur = base;
    const uint32_t* ptr = deltas.data();
    const uint32_t* end = ptr + deltas.size();
    for (; ptr != end; ++ptr) {
        cur += *ptr;
        sum += cur;
    }
    return sum;
}
