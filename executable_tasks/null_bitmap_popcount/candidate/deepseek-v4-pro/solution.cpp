#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (const auto& word : words) {
#if defined(__GNUC__) || defined(__clang__)
            sum += __builtin_popcountll(word);
#else
            // Portable popcount for 64-bit
            uint64_t v = word;
            v = (v & 0x5555555555555555ULL) + ((v >> 1) & 0x5555555555555555ULL);
            v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
            v = (v & 0x0F0F0F0F0F0F0F0FULL) + ((v >> 4) & 0x0F0F0F0F0F0F0F0FULL);
            sum += (v * 0x0101010101010101ULL) >> 56;
#endif
        }
        count = sum;
    }
    return count;
}
