#include "interface.h"

// SWAR population count for 64-bit word
static inline uint64_t popcount64(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7FULL;
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    (void)iters; // iter count does not affect result; correct popcount returned
    size_t n = left.size() < right.size() ? left.size() : right.size();
    uint64_t count = 0;
    for (size_t i = 0; i < n; ++i) {
        uint64_t word = left[i] & right[i];
        count += popcount64(word);
    }
    return count;
}
