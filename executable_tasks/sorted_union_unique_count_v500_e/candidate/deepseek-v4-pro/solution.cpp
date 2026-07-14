#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    (void)iters; // unused
    size_t i = 0, j = 0;
    size_t n = left.size(), m = right.size();
    uint64_t count = 0;
    uint32_t last_val = 0;
    bool first = true;
    while (i < n || j < m) {
        uint32_t val;
        if (i < n && (j >= m || left[i] < right[j])) {
            val = left[i];
            ++i;
        } else if (j < m && (i >= n || right[j] < left[i])) {
            val = right[j];
            ++j;
        } else { // equal values
            val = left[i];
            ++i;
            ++j;
        }
        if (first || val != last_val) {
            ++count;
            last_val = val;
            first = false;
        }
    }
    return count;
}