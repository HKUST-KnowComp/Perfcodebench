#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    const std::size_t n = left.size() < right.size() ? left.size() : right.size();
    const uint64_t* const l = left.data();
    const uint64_t* const r = right.data();
    uint64_t count = 0;
    std::size_t i = 0;

    // Unroll by 4 to exploit ILP; modern CPUs can execute multiple popcnts in parallel.
    for (; i + 4 <= n; i += 4) {
        count += static_cast<uint64_t>(__builtin_popcountll(l[i]     & r[i]));
        count += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
        count += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
        count += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
    }
    for (; i < n; ++i) {
        count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    }

    return count;
}
