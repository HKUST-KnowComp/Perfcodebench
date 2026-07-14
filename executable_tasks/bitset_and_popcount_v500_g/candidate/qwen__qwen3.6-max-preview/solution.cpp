#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    const uint64_t* l = left.data();
    const uint64_t* r = right.data();
    const size_t n = left.size();
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            c0 += __builtin_popcountll(l[i] & r[i]);
            c1 += __builtin_popcountll(l[i+1] & r[i+1]);
            c2 += __builtin_popcountll(l[i+2] & r[i+2]);
            c3 += __builtin_popcountll(l[i+3] & r[i+3]);
        }
        for (; i < n; ++i) {
            c0 += __builtin_popcountll(l[i] & r[i]);
        }
        count = c0 + c1 + c2 + c3;
    }
    return count;
}
