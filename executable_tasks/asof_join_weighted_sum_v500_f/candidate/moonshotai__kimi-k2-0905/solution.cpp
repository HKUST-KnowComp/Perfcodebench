#include "interface.h"

#include <immintrin.h>
#include <algorithm>

static inline uint32_t fast_lower(const uint32_t* __restrict arr, uint32_t n, uint32_t key) {
    uint32_t lo = 0, hi = n;
    while (lo < hi) {
        uint32_t mid = (lo + hi) >> 1;
        if (arr[mid] <= key) lo = mid + 1;
        else hi = mid;
    }
    return lo - 1;
}

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const uint32_t* ts = dim_ts.data();
    const uint32_t* vs = dim_values.data();
    const uint32_t* pts = probe_ts.data();
    const uint32_t* pws = probe_weights.data();
    const std::size_t dsize = dim_ts.size();
    const std::size_t psize = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;

        // unroll 4-way
        for (; i + 3 < psize; i += 4) {
            _mm_prefetch((const char*)(ts + 64), _MM_HINT_T0);
            uint32_t k0 = pts[i];
            uint32_t k1 = pts[i + 1];
            uint32_t k2 = pts[i + 2];
            uint32_t k3 = pts[i + 3];

            uint32_t idx0 = fast_lower(ts, dsize, k0);
            uint32_t idx1 = fast_lower(ts, dsize, k1);
            uint32_t idx2 = fast_lower(ts, dsize, k2);
            uint32_t idx3 = fast_lower(ts, dsize, k3);

            sum += uint64_t(vs[idx0]) * uint64_t(pws[i]);
            sum += uint64_t(vs[idx1]) * uint64_t(pws[i + 1]);
            sum += uint64_t(vs[idx2]) * uint64_t(pws[i + 2]);
            sum += uint64_t(vs[idx3]) * uint64_t(pws[i + 3]);
        }
        for (; i < psize; ++i) {
            uint32_t idx = fast_lower(ts, dsize, pts[i]);
            sum += uint64_t(vs[idx]) * uint64_t(pws[i]);
        }
    }
    return sum;
}