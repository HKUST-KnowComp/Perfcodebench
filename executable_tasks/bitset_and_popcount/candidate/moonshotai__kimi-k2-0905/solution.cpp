#include "interface.h"
#include <cstdint>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#define POPCOUNT __popcnt64
#else
#define POPCOUNT __builtin_popcountll
#endif

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters)
{
    const std::size_t n = left.size();
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        for (std::size_t i = 0; i < n; ++i) {
            cnt += POPCOUNT(left[i] & right[i]);
        }
        total = cnt;
    }
    return total;
}