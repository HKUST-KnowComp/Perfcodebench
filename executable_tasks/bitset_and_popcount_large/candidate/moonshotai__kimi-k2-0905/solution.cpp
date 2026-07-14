#include "interface.h"
#include <cstdint>
#include <vector>

// 4-bit Harley-Seal popcount kernel: 3 ALU ops / word, no multiplies.
// Returns population count of val.
static inline uint64_t popcount_u64(uint64_t val) {
    val -= (val >> 1) & 0x5555555555555555ULL;
    val  = (val & 0x3333333333333333ULL) + ((val >> 2) & 0x3333333333333333ULL);
    val  = (val + (val >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (val * 0x0101010101010101ULL) >> 56;
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters)
{
    const std::size_t n = left.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        std::size_t i = 0;

        // Process 4 words at a time for better ILP and auto-vectorization.
        for (; i + 3 < n; i += 4) {
            uint64_t w0 = left[i+0] & right[i+0];
            uint64_t w1 = left[i+1] & right[i+1];
            uint64_t w2 = left[i+2] & right[i+2];
            uint64_t w3 = left[i+3] & right[i+3];
            cnt += popcount_u64(w0);
            cnt += popcount_u64(w1);
            cnt += popcount_u64(w2);
            cnt += popcount_u64(w3);
        }
        // Handle tail < 4 words.
        for (; i < n; ++i) {
            cnt += popcount_u64(left[i] & right[i]);
        }
        total = cnt;   // keep side-effect on iter
    }
    return total;
}