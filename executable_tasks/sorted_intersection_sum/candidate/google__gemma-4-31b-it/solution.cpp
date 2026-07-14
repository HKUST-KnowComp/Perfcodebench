#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
    if (iters <= 0) return 0;

    uint64_t single_sum = 0;
    size_t i = 0, j = 0;
    const size_t n = lhs.size();
    const size_t m = rhs.size();
    const uint32_t* l_ptr = lhs.data();
    const uint32_t* r_ptr = rhs.data();

    while (i < n && j < m) {
        if (l_ptr[i] < r_ptr[j]) {
            i++;
        } else if (l_ptr[i] > r_ptr[j]) {
            j++;
        } else {
            single_sum += l_ptr[i];
            i++;
            j++;
        }
    }

    return single_sum * static_cast<uint64_t>(iters);
}