#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    const uint32_t* const l = left.data();
    const uint32_t* const r = right.data();
    const size_t n = left.size();
    const size_t m = right.size();

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t cnt = 0;

        // Main 4-element unrolled loop
        while (i + 3 < n && j + 3 < m) {
            uint32_t a0 = l[i], a1 = l[i+1], a2 = l[i+2], a3 = l[i+3];
            uint32_t b0 = r[j], b1 = r[j+1], b2 = r[j+2], b3 = r[j+3];

            if (a0 < b0) { cnt++; if (a1 != a0) cnt++; if (a2 != a1 && a2 != a0) cnt++; if (a3 != a2 && a3 != a1 && a3 != a0) cnt++; i += 4; }
            else if (b0 < a0) { cnt++; if (b1 != b0) cnt++; if (b2 != b1 && b2 != b0) cnt++; if (b3 != b2 && b3 != b1 && b3 != b0) cnt++; j += 4; }
            else { cnt++; if (a1 != a0) cnt++; if (a2 != a1 && a2 != a0) cnt++; if (a3 != a2 && a3 != a1 && a3 != a0) cnt++; i += 4; j += 4; }
        }

        // Scalar tail
        while (i < n && j < m) {
            if (l[i] < r[j]) { cnt += (i == 0 || l[i] != l[i-1]); i++; }
            else if (r[j] < l[i]) { cnt += (j == 0 || r[j] != r[j-1]); j++; }
            else { cnt += (i == 0 || l[i] != l[i-1]); i++; j++; }
        }
        while (i < n) { cnt += (i == 0 || l[i] != l[i-1]); i++; }
        while (j < m) { cnt += (j == 0 || r[j] != r[j-1]); j++; }

        count = cnt;
    }
    return count;
}