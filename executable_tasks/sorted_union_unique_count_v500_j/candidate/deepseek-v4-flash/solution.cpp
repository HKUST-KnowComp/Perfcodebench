#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    (void)iters;  // unused, single pass is enough
    uint64_t count = 0;
    uint32_t prev = 0;
    bool first = true;
    size_t i = 0, j = 0;

    while (i < left.size() && j < right.size()) {
        uint32_t val;
        if (left[i] < right[j]) {
            val = left[i];
            ++i;
        } else if (left[i] > right[j]) {
            val = right[j];
            ++j;
        } else {
            val = left[i];
            ++i;
            ++j;
        }
        if (first || val != prev) {
            ++count;
            prev = val;
            first = false;
        }
    }
    while (i < left.size()) {
        uint32_t val = left[i];
        ++i;
        if (first || val != prev) {
            ++count;
            prev = val;
            first = false;
        }
    }
    while (j < right.size()) {
        uint32_t val = right[j];
        ++j;
        if (first || val != prev) {
            ++count;
            prev = val;
            first = false;
        }
    }
    return count;
}
