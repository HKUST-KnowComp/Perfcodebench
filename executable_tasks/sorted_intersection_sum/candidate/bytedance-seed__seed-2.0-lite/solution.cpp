#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
    uint64_t sum = 0;
    const size_t n = lhs.size();
    const size_t m = rhs.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0, j = 0;
        while (i < n && j < m) {
            const uint32_t a = lhs[i];
            const uint32_t b = rhs[j];
            if (a == b) {
                sum += a;
                ++i;
                ++j;
            } else if (a < b) {
                ++i;
            } else {
                ++j;
            }
        }
    }
    return sum;
}