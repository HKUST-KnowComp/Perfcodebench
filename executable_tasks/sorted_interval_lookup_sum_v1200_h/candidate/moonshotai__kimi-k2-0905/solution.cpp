#include "interface.h"
#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const uint32_t*  s = starts.data();
    const uint32_t*  e = ends.data();
    const uint32_t*  p = payloads.data();
    const uint32_t*  q = probes.data();
    const uint32_t*  w = weights.data();
    const size_t     n = starts.size();
    const size_t     m = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;
        // unroll 4x to increase ILP
        for (; i + 3 < m; i += 4) {
            uint32_t q0 = q[i+0], q1 = q[i+1], q2 = q[i+2], q3 = q[i+3];
            uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;

            // ---- binary search for q0 ------------------------------------
            size_t lo = 0, hi = n;
            while (hi - lo > 1) {
                size_t mid = (lo + hi) >> 1;
                if (s[mid] <= q0) lo = mid;
                else              hi = mid;
            }
            if (q0 < e[lo]) acc0 = uint64_t(p[lo]) * uint64_t(w[i+0]);

            // ---- binary search for q1 ------------------------------------
            lo = 0; hi = n;
            while (hi - lo > 1) {
                size_t mid = (lo + hi) >> 1;
                if (s[mid] <= q1) lo = mid;
                else              hi = mid;
            }
            if (q1 < e[lo]) acc1 = uint64_t(p[lo]) * uint64_t(w[i+1]);

            // ---- binary search for q2 ------------------------------------
            lo = 0; hi = n;
            while (hi - lo > 1) {
                size_t mid = (lo + hi) >> 1;
                if (s[mid] <= q2) lo = mid;
                else              hi = mid;
            }
            if (q2 < e[lo]) acc2 = uint64_t(p[lo]) * uint64_t(w[i+2]);

            // ---- binary search for q3 ------------------------------------
            lo = 0; hi = n;
            while (hi - lo > 1) {
                size_t mid = (lo + hi) >> 1;
                if (s[mid] <= q3) lo = mid;
                else              hi = mid;
            }
            if (q3 < e[lo]) acc3 = uint64_t(p[lo]) * uint64_t(w[i+3]);

            sum += acc0 + acc1 + acc2 + acc3;
        }
        // tail
        for (; i < m; ++i) {
            uint32_t qi = q[i];
            size_t lo = 0, hi = n;
            while (hi - lo > 1) {
                size_t mid = (lo + hi) >> 1;
                if (s[mid] <= qi) lo = mid;
                else              hi = mid;
            }
            if (qi < e[lo]) sum += uint64_t(p[lo]) * uint64_t(w[i]);
        }
    }
    return sum;
}