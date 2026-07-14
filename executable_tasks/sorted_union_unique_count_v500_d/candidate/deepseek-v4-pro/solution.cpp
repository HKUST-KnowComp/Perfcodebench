#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    size_t i = 0, j = 0;
    size_t n = left.size(), m = right.size();
    uint64_t count = 0;

    while (i < n && j < m) {
        if (left[i] < right[j]) {
            uint32_t val = left[i];
            ++count;
            do { ++i; } while (i < n && left[i] == val);
        } else if (right[j] < left[i]) {
            uint32_t val = right[j];
            ++count;
            do { ++j; } while (j < m && right[j] == val);
        } else {
            uint32_t val = left[i];
            ++count;
            do { ++i; } while (i < n && left[i] == val);
            do { ++j; } while (j < m && right[j] == val);
        }
    }
    while (i < n) {
        uint32_t val = left[i];
        ++count;
        do { ++i; } while (i < n && left[i] == val);
    }
    while (j < m) {
        uint32_t val = right[j];
        ++count;
        do { ++j; } while (j < m && right[j] == val);
    }
    return count;
}