#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    const uint32_t *l = left.data();
    const uint32_t *r = right.data();
    size_t n = left.size();
    size_t m = right.size();

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t local = 0;

        /* Unrolled 4× merge with minimal branching */
        while (i + 3 < n && j + 3 < m) {
            uint32_t a0 = l[i],   a1 = l[i+1], a2 = l[i+2], a3 = l[i+3];
            uint32_t b0 = r[j],   b1 = r[j+1], b2 = r[j+2], b3 = r[j+3];

            if (a3 < b0) {                 /* a0..a3 all smaller */
                local += (a0 != a1) + (a1 != a2) + (a2 != a3) + 1;
                i += 4;
            } else if (b3 < a0) {          /* b0..b3 all smaller */
                local += (b0 != b1) + (b1 != b2) + (b2 != b3) + 1;
                j += 4;
            } else {                       /* overlap: process one by one */
                uint32_t v = (a0 < b0) ? a0 : b0;
                local++;
                i += (a0 == v);
                j += (b0 == v);
                /* skip duplicates */
                while (i < n && l[i] == v) ++i;
                while (j < m && r[j] == v) ++j;
            }
        }

        /* Scalar tail for remaining elements */
        while (i < n && j < m) {
            uint32_t v = (l[i] < r[j]) ? l[i] : r[j];
            local++;
            i += (l[i] == v);
            j += (r[j] == v);
            while (i < n && l[i] == v) ++i;
            while (j < m && r[j] == v) ++j;
        }

        /* Drain leftovers */
        while (i < n) {
            uint32_t v = l[i++];
            local++;
            while (i < n && l[i] == v) ++i;
        }
        while (j < m) {
            uint32_t v = r[j++];
            local++;
            while (j < m && r[j] == v) ++j;
        }

        count = local;
    }
    return count;
}