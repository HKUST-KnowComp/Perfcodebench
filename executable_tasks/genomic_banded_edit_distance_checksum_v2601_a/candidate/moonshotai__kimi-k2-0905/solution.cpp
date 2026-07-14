#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <immintrin.h>

namespace {

static inline int banded_distance(const std::string& a,
                                  const std::string& b,
                                  int k) noexcept {
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    if (std::abs(n - m) > k) return -1;           // impossible within band
    if (a == b) return 0;

    // Ensure b is the shorter string to minimise memory.
    if (n < m) return banded_distance(b, a, k);

    constexpr int ALIGN = 32;
    alignas(ALIGN) int buf[2][2 * 1024 + 64];     // safe for k<=1024
    int* prev = buf[0];
    int* cur  = buf[1];

    const int width = 2 * k + 1;
    const int diag0 = k;             // logical diagonal 0 maps here

    // Initialise first row.
    for (int j = 0; j <= k && j <= m; ++j) prev[diag0 + j] = j;

    for (int i = 1; i <= n; ++i) {
        const int left_diag  = std::max(-k, -i);
        const int right_diag = std::min(k, m - i);

        const int left  = diag0 + left_diag;
        const int right = diag0 + right_diag;

        const char ca = a[i - 1];

        // SIMD load 4 chars of b once per outer iteration.
        const __m128i ca16 = _mm_set1_epi8(ca);

        // Handle leftmost cell explicitly.
        if (i - 1 >= 0 && left_diag <= -i) {
            cur[left] = i;
        } else {
            cur[left] = prev[left + 1] + 1;
        }

        // Vectorised inner loop: process four diagonals at a time.
        int j = left_diag + 1;
        for (; j + 3 <= right_diag; j += 4) {
            const int idx[4] = { i + j - 1, i + j, i + j + 1, i + j + 2 };
            const __m128i bvec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b[idx[0]]));
            const __m128i eq   = _mm_cmpeq_epi8(ca16, bvec);
            const int mask     = _mm_movemask_epi8(eq);

            const int d0 = prev[diag0 + j - 1] + ((mask & 1) ? 0 : 1);
            const int d1 = prev[diag0 + j]     + ((mask & 2) ? 0 : 1);
            const int d2 = prev[diag0 + j + 1] + ((mask & 4) ? 0 : 1);
            const int d3 = prev[diag0 + j + 2] + ((mask & 8) ? 0 : 1);

            const int ins0 = cur[diag0 + j - 1] + 1;
            const int ins1 = cur[diag0 + j]     + 1;
            const int ins2 = cur[diag0 + j + 1] + 1;
            const int ins3 = cur[diag0 + j + 2] + 1;

            const int del0 = prev[diag0 + j]     + 1;
            const int del1 = prev[diag0 + j + 1] + 1;
            const int del2 = prev[diag0 + j + 2] + 1;
            const int del3 = prev[diag0 + j + 3] + 1;

            cur[diag0 + j]     = std::min({d0, ins0, del0});
            cur[diag0 + j + 1] = std::min({d1, ins1, del1});
            cur[diag0 + j + 2] = std::min({d2, ins2, del2});
            cur[diag0 + j + 3] = std::min({d3, ins3, del3});
        }

        // Tail scalar loop.
        for (; j <= right_diag; ++j) {
            const int cost = (ca == b[i + j - 1]) ? 0 : 1;
            const int sub  = prev[diag0 + j - 1] + cost;
            const int ins  = cur[diag0 + j - 1] + 1;
            const int del  = prev[diag0 + j]   + 1;
            cur[diag0 + j] = std::min({sub, ins, del});
        }

        // Early exit if the band cannot reach zero.
        if (cur[diag0] > k && i + k < n) return -1;

        std::swap(prev, cur);
    }

    const int result = prev[diag0 + (m - n)];
    return (result <= k) ? result : -1;
}

} // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
    const std::size_t n = lhs.size();
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            int d = banded_distance(lhs[i], rhs[i], max_band);
            if (d < 0) d = max_band + 1; // fallback when band insufficient
            hash ^= static_cast<uint64_t>(d + 1);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}