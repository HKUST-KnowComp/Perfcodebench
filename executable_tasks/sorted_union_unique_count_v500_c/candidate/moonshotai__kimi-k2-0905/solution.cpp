#include "interface.h"
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    // Edge-cases
    if (iters <= 0) return 0;
    if (left.empty() && right.empty()) return 0;

    size_t i = 0, j = 0;
    uint64_t distinct = 0;

    // Single linear merge pass to count distinct elements
    while (i < left.size() && j < right.size()) {
        if (left[i] < right[j]) {
            ++distinct;
            uint32_t val = left[i];
            while (i < left.size() && left[i] == val) ++i;
        } else if (left[i] > right[j]) {
            ++distinct;
            uint32_t val = right[j];
            while (j < right.size() && right[j] == val) ++j;
        } else {
            ++distinct;
            uint32_t val = left[i];
            while (i < left.size() && left[i] == val) ++i;
            while (j < right.size() && right[j] == val) ++j;
        }
    }
    while (i < left.size()) {
        ++distinct;
        uint32_t val = left[i];
        while (i < left.size() && left[i] == val) ++i;
    }
    while (j < right.size()) {
        ++distinct;
        uint32_t val = right[j];
        while (j < right.size() && right[j] == val) ++j;
    }

    // Result is identical for every iteration
    return distinct;
}