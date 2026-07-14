#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
    uint64_t sum = 0;
    const uint32_t* l_ptr = lhs.data();
    const uint32_t* r_ptr = rhs.data();
    const size_t l_size = lhs.size();
    const size_t r_size = rhs.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0, j = 0;
        while (i < l_size && j < r_size) {
            if (l_ptr[i] == r_ptr[j]) {
                sum += static_cast<uint64_t>(l_ptr[i]);
                ++i;
                ++j;
            } else if (l_ptr[i] < r_ptr[j]) {
                ++i;
            } else {
                ++j;
            }
        }
    }
    return sum;
}