#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t sorted_union_count(const std::vector<uint32_t>& left, const std::vector<uint32_t>& right, int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t current_count = 0;
        uint32_t last_val = 0;
        bool first = true;

        while (i < left.size() || j < right.size()) {
            uint32_t val;
            if (i < left.size() && (j == right.size() || left[i] < right[j])) {
                val = left[i++];
            } else if (j < right.size() && (i == left.size() || right[j] < left[i])) {
                val = right[j++];
            } else {
                val = left[i++];
                j++;
            }

            if (first || val != last_val) {
                current_count++;
                last_val = val;
                first = false;
            }
        }
        count = current_count;
    }
    return count;
}