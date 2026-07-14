#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        size_t n = left.size(), m = right.size();
        uint64_t local_count = 0;
        while (i < n && j < m) {
            if (left[i] < right[j]) {
                uint32_t val = left[i];
                ++local_count;
                ++i;
                while (i < n && left[i] == val) ++i;
            } else if (left[i] > right[j]) {
                uint32_t val = right[j];
                ++local_count;
                ++j;
                while (j < m && right[j] == val) ++j;
            } else {
                uint32_t val = left[i];
                ++local_count;
                ++i;
                ++j;
                while (i < n && left[i] == val) ++i;
                while (j < m && right[j] == val) ++j;
            }
        }
        while (i < n) {
            uint32_t val = left[i];
            ++local_count;
            ++i;
            while (i < n && left[i] == val) ++i;
        }
        while (j < m) {
            uint32_t val = right[j];
            ++local_count;
            ++j;
            while (j < m && right[j] == val) ++j;
        }
        count = local_count;
    }
    return count;
}