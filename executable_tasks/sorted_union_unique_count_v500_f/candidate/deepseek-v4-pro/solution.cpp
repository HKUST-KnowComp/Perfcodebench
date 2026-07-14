#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
    uint64_t count = 0;
    bool has_prev = false;
    uint32_t prev = 0;
    size_t i = 0, j = 0;
    const size_t n = left.size(), m = right.size();

    while (i < n && j < m) {
        uint32_t val;
        if (left[i] < right[j]) {
            val = left[i];
            while (i < n && left[i] == val) ++i;
        } else if (left[i] > right[j]) {
            val = right[j];
            while (j < m && right[j] == val) ++j;
        } else {
            val = left[i];
            while (i < n && left[i] == val) ++i;
            while (j < m && right[j] == val) ++j;
        }
        if (!has_prev || val != prev) {
            count++;
            prev = val;
            has_prev = true;
        }
    }
    while (i < n) {
        uint32_t val = left[i];
        while (i < n && left[i] == val) ++i;
        if (!has_prev || val != prev) {
            count++;
            prev = val;
            has_prev = true;
        }
    }
    while (j < m) {
        uint32_t val = right[j];
        while (j < m && right[j] == val) ++j;
        if (!has_prev || val != prev) {
            count++;
            prev = val;
            has_prev = true;
        }
    }
    return count;
}
