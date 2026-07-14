#include "interface.h"
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Two-pointer merge and count distinct values in union
        auto i = left.begin(), j = right.begin();
        auto end_i = left.end(), end_j = right.end();
        uint64_t cur_count = 0;
        uint32_t last = 0; // used only when cur_count > 0

        while (i != end_i && j != end_j) {
            if (*i < *j) {
                if (cur_count == 0 || *i != last) {
                    last = *i;
                    ++cur_count;
                }
                ++i;
            } else if (*j < *i) {
                if (cur_count == 0 || *j != last) {
                    last = *j;
                    ++cur_count;
                }
                ++j;
            } else { // equal
                if (cur_count == 0 || *i != last) {
                    last = *i;
                    ++cur_count;
                }
                ++i;
                ++j;
            }
        }

        // Process remaining elements of left
        while (i != end_i) {
            if (cur_count == 0 || *i != last) {
                last = *i;
                ++cur_count;
            }
            ++i;
        }

        // Process remaining elements of right
        while (j != end_j) {
            if (cur_count == 0 || *j != last) {
                last = *j;
                ++cur_count;
            }
            ++j;
        }

        count = cur_count;
    }
    return count;
}
