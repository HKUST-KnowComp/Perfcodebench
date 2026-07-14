#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    std::size_t n = left.size();
    if (right.size() < n) {
        n = right.size();
    }

    const uint64_t* l = left.data();
    const uint64_t* r = right.data();

    uint64_t count = 0;
    std::size_t i = 0;

    uint64_t a0 = 0, a1 = 0, a2 = 0, a3 = 0;
    uint64_t a4 = 0, a5 = 0, a6 = 0, a7 = 0;

    for (; i + 8 <= n; i += 8) {
        a0 += static_cast<uint64_t>(__builtin_popcountll(l[i + 0] & r[i + 0]));
        a1 += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
        a2 += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
        a3 += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
        a4 += static_cast<uint64_t>(__builtin_popcountll(l[i + 4] & r[i + 4]));
        a5 += static_cast<uint64_t>(__builtin_popcountll(l[i + 5] & r[i + 5]));
        a6 += static_cast<uint64_t>(__builtin_popcountll(l[i + 6] & r[i + 6]));
        a7 += static_cast<uint64_t>(__builtin_popcountll(l[i + 7] & r[i + 7]));
    }

    count = a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7;

    for (; i < n; ++i) {
        count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    }

    return count;
}
