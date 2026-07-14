#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        const size_t n = left.size(), m = right.size();
        count = 0;
        while (i < n && j < m) {
            uint32_t v;
            if (left[i] < right[j]) {
                v = left[i++];
            } else if (left[i] > right[j]) {
                v = right[j++];
            } else {
                v = left[i];
                ++i; ++j;
            }
            ++count;
            // skip duplicates in left
            while (i < n && left[i] == v) ++i;
            // skip duplicates in right
            while (j < m && right[j] == v) ++j;
        }
        // consume remaining tail in left
        while (i < n) {
            uint32_t v = left[i++];
            ++count;
            while (i < n && left[i] == v) ++i;
        }
        // consume remaining tail in right
        while (j < m) {
            uint32_t v = right[j++];
            ++count;
            while (j < m && right[j] == v) ++j;
        }
    }
    return count;
}