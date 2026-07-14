#include "interface.h"

#include <vector>

static inline uint64_t popcount64(uint64_t x) {
#ifdef __GNUC__
    return static_cast<uint64_t>(__builtin_popcountll(x));
#else
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    const std::size_t n = left.size() < right.size() ? left.size() : right.size();

    std::size_t i = 0;
    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    for (; i + 3 < n; i += 4) {
        c0 += popcount64(left[i]     & right[i]);
        c1 += popcount64(left[i + 1] & right[i + 1]);
        c2 += popcount64(left[i + 2] & right[i + 2]);
        c3 += popcount64(left[i + 3] & right[i + 3]);
    }
    uint64_t count = c0 + c1 + c2 + c3;

    for (; i < n; ++i) {
        count += popcount64(left[i] & right[i]);
    }

    return count;
}
