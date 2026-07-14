#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs,
                                 const std::vector<uint32_t>& rhs,
                                 int iters)
{
    const uint32_t* const l_data = lhs.data();
    const uint32_t* const r_data = rhs.data();
    const size_t l_size = lhs.size();
    const size_t r_size = rhs.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0, j = 0;
        while (i < l_size && j < r_size) {
            const uint32_t lv = l_data[i];
            const uint32_t rv = r_data[j];
            if (lv == rv) {
                sum += static_cast<uint64_t>(lv);
                ++i;
                ++j;
            } else if (lv < rv) {
                ++i;
            } else {
                ++j;
            }
        }
    }
    return sum;
}