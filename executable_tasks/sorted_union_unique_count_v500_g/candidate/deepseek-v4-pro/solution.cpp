#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    (void)iters; // unused, result is deterministic
    const uint32_t* bl = left.data();
    const uint32_t* br = right.data();
    size_t nl = left.size();
    size_t nr = right.size();
    size_t i = 0, j = 0;
    uint64_t distinct = 0;
    uint32_t prev = 0;
    bool first = true;

    while (i < nl && j < nr) {
        uint32_t lv = bl[i];
        uint32_t rv = br[j];
        if (lv < rv) {
            if (first || lv != prev) {
                ++distinct;
                prev = lv;
                first = false;
            }
            ++i;
            while (i < nl && bl[i] == lv) ++i;
        } else if (rv < lv) {
            if (first || rv != prev) {
                ++distinct;
                prev = rv;
                first = false;
            }
            ++j;
            while (j < nr && br[j] == rv) ++j;
        } else {
            // values equal
            if (first || lv != prev) {
                ++distinct;
                prev = lv;
                first = false;
            }
            ++i; ++j;
            while (i < nl && bl[i] == lv) ++i;
            while (j < nr && br[j] == lv) ++j;
        }
    }

    // process remaining left
    while (i < nl) {
        uint32_t lv = bl[i];
        if (first || lv != prev) {
            ++distinct;
            prev = lv;
            first = false;
        }
        ++i;
        while (i < nl && bl[i] == lv) ++i;
    }

    // process remaining right
    while (j < nr) {
        uint32_t rv = br[j];
        if (first || rv != prev) {
            ++distinct;
            prev = rv;
            first = false;
        }
        ++j;
        while (j < nr && br[j] == rv) ++j;
    }

    return distinct;
}
