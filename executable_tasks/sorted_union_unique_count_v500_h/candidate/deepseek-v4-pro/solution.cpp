#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t count = 0;
    size_t i = 0, j = 0;
    const size_t lsize = left.size();
    const size_t rsize = right.size();

    while (i < lsize && j < rsize) {
        uint32_t lval = left[i];
        uint32_t rval = right[j];
        if (lval < rval) {
            ++count;
            do { ++i; } while (i < lsize && left[i] == lval);
        } else if (rval < lval) {
            ++count;
            do { ++j; } while (j < rsize && right[j] == rval);
        } else {
            ++count;
            do { ++i; } while (i < lsize && left[i] == lval);
            do { ++j; } while (j < rsize && right[j] == rval);
        }
    }

    while (i < lsize) {
        uint32_t lval = left[i];
        ++count;
        do { ++i; } while (i < lsize && left[i] == lval);
    }

    while (j < rsize) {
        uint32_t rval = right[j];
        ++count;
        do { ++j; } while (j < rsize && right[j] == rval);
    }

    return count;
}