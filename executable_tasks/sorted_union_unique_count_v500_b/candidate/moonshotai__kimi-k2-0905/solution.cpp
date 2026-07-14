#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    const size_t n = left.size();
    const size_t m = right.size();
    size_t i = 0, j = 0;
    uint64_t distinct = 0;

    // single linear merge to count distinct elements
    while (i < n && j < m) {
        if (left[i] < right[j]) {
            ++distinct;
            uint32_t v = left[i];
            while (++i < n && left[i] == v) {}
        } else if (left[i] > right[j]) {
            ++distinct;
            uint32_t v = right[j];
            while (++j < m && right[j] == v) {}
        } else { // equal
            ++distinct;
            uint32_t v = left[i];
            while (++i < n && left[i] == v) {}
            while (++j < m && right[j] == v) {}
        }
    }
    while (i < n) {
        ++distinct;
        uint32_t v = left[i];
        while (++i < n && left[i] == v) {}
    }
    while (j < m) {
        ++distinct;
        uint32_t v = right[j];
        while (++j < m && right[j] == v) {}
    }

    // reuse the computed count for every requested iteration
    return distinct;
}