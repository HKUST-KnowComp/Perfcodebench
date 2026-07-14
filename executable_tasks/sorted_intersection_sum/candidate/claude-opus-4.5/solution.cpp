#include "interface.h"

#include <cstdint>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
    uint64_t sum = 0;
    const uint32_t* __restrict__ l = lhs.data();
    const uint32_t* __restrict__ r = rhs.data();
    const uint32_t* const l_end = l + lhs.size();
    const uint32_t* const r_end = r + rhs.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint32_t* li = l;
        const uint32_t* ri = r;

        while (li < l_end && ri < r_end) {
            uint32_t lv = *li;
            uint32_t rv = *ri;
            // Branch-free: add to sum only if equal
            sum += static_cast<uint64_t>(lv) * (lv == rv);
            // Advance pointers: li if lv <= rv, ri if rv <= lv
            li += (lv <= rv);
            ri += (rv <= lv);
        }
    }
    return sum;
}
