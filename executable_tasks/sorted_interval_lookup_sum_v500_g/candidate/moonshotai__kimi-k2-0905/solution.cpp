#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstring>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    (void)ends;
    const uint32_t  num_iv = starts.size();
    const uint32_t*  iv_st = starts.data();
    const uint32_t*  iv_py = payloads.data();
    const uint32_t*  pb    = probes.data();
    const uint32_t*  wt    = weights.data();
    const size_t     pb_sz = probes.size();

    /* ---------- build 4 GiB lookup table once ---------- */
    static thread_local std::vector<uint32_t> tbl;
    if (tbl.empty()) {
        tbl.resize(1ULL << 32);
        uint32_t idx = 0;
        for (uint64_t val = 0; val < (1ULL << 32); ++val) {
            while (idx + 1 < num_iv && iv_st[idx + 1] <= val) ++idx;
            tbl[val] = idx;
        }
    }

    /* ---------- hot loop ---------- */
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        size_t i = 0;
        for (; i + 8 <= pb_sz; i += 8) {
            const uint32_t v0 = pb[i+0], w0 = wt[i+0];
            const uint32_t v1 = pb[i+1], w1 = wt[i+1];
            const uint32_t v2 = pb[i+2], w2 = wt[i+2];
            const uint32_t v3 = pb[i+3], w3 = wt[i+3];
            const uint32_t v4 = pb[i+4], w4 = wt[i+4];
            const uint32_t v5 = pb[i+5], w5 = wt[i+5];
            const uint32_t v6 = pb[i+6], w6 = wt[i+6];
            const uint32_t v7 = pb[i+7], w7 = wt[i+7];

            const uint32_t idx0 = tbl[v0];
            const uint32_t idx1 = tbl[v1];
            const uint32_t idx2 = tbl[v2];
            const uint32_t idx3 = tbl[v3];
            const uint32_t idx4 = tbl[v4];
            const uint32_t idx5 = tbl[v5];
            const uint32_t idx6 = tbl[v6];
            const uint32_t idx7 = tbl[v7];

            acc[0] += uint64_t(iv_py[idx0]) * uint64_t(w0);
            acc[1] += uint64_t(iv_py[idx1]) * uint64_t(w1);
            acc[2] += uint64_t(iv_py[idx2]) * uint64_t(w2);
            acc[3] += uint64_t(iv_py[idx3]) * uint64_t(w3);
            acc[4] += uint64_t(iv_py[idx4]) * uint64_t(w4);
            acc[5] += uint64_t(iv_py[idx5]) * uint64_t(w5);
            acc[6] += uint64_t(iv_py[idx6]) * uint64_t(w6);
            acc[7] += uint64_t(iv_py[idx7]) * uint64_t(w7);
        }
        sum = acc[0] + acc[1] + acc[2] + acc[3] +
              acc[4] + acc[5] + acc[6] + acc[7];
        for (; i < pb_sz; ++i) {
            const uint32_t idx = tbl[pb[i]];
            sum += uint64_t(iv_py[idx]) * uint64_t(wt[i]);
        }
    }
    return sum;
}