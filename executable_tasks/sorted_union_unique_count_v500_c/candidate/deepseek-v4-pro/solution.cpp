#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t count = 0;
    size_t i = 0, j = 0;
    uint32_t prev = 0;
    bool first = true;
    while (i < left.size() && j < right.size()) {
        uint32_t val;
        if (left[i] < right[j]) {
            val = left[i++];
        } else if (left[i] > right[j]) {
            val = right[j++];
        } else {
            val = left[i];
            ++i; ++j;
        }
        if (first || val != prev) {
            prev = val;
            ++count;
            first = false;
        }
    }
    while (i < left.size()) {
        uint32_t val = left[i++];
        if (first || val != prev) {
            prev = val;
            ++count;
            first = false;
        }
    }
    while (j < right.size()) {
        uint32_t val = right[j++];
        if (first || val != prev) {
            prev = val;
            ++count;
            first = false;
        }
    }
    return count;
}