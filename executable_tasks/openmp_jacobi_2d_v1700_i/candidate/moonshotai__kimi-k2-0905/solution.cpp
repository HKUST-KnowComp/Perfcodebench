#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

inline uint64_t checksum_u32(const uint32_t* __restrict data, std::size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    const int R = rows;
    const int C = cols;
    const int S = steps;

    std::vector<uint32_t> buf0(input);
    std::vector<uint32_t> buf1(input.size());

    uint32_t* __restrict a = buf0.data();
    uint32_t* __restrict b = buf1.data();

    const int C2 = C + C; // 2*C

    #pragma omp parallel
    {
        for (int s = 0; s < S; ++s) {
            #pragma omp for schedule(dynamic, 64)
            for (int row = 1; row < R - 1; ++row) {
                const uint32_t* __restrict ar = a + (std::size_t)row * C;
                uint32_t* __restrict br = b + (std::size_t)row * C;

                // left ghost
                br[0] = ar[0];
                // right ghost
                br[C - 1] = ar[C - 1];

                // inner 4-way unrolled
                int col = 1;
                const int end4 = C - 1 - 3;
                for (; col < end4; col += 4) {
                    const uint32_t v0 = ar[col];
                    const uint32_t v1 = ar[col + 1];
                    const uint32_t v2 = ar[col + 2];
                    const uint32_t v3 = ar[col + 3];

                    const uint32_t s0 = ar[col - 1] + ar[col + 1];
                    const uint32_t s1 = ar[col]     + ar[col + 2];
                    const uint32_t s2 = ar[col + 1] + ar[col + 3];
                    const uint32_t s3 = ar[col + 2] + ar[col + 4];

                    const uint32_t u0 = ar[col - C] + ar[col + C];
                    const uint32_t u1 = ar[col - C + 1] + ar[col + C + 1];
                    const uint32_t u2 = ar[col - C + 2] + ar[col + C + 2];
                    const uint32_t u3 = ar[col - C + 3] + ar[col + C + 3];

                    br[col]     = (v0 * 4u + s0 + u0) >> 3;
                    br[col + 1] = (v1 * 4u + s1 + u1) >> 3;
                    br[col + 2] = (v2 * 4u + s2 + u2) >> 3;
                    br[col + 3] = (v3 * 4u + s3 + u3) >> 3;
                }
                for (; col < C - 1; ++col) {
                    const uint32_t v = ar[col];
                    const uint32_t s = ar[col - 1] + ar[col + 1];
                    const uint32_t u = ar[col - C] + ar[col + C];
                    br[col] = (v * 4u + s + u) >> 3;
                }
            }

            // ghost rows (top/bottom)
            #pragma omp single
            {
                for (int c = 0; c < C; ++c) {
                    b[c] = a[c];
                    b[(R - 1) * C + c] = a[(R - 1) * C + c];
                }
            }

            // swap pointers
            #pragma omp single
            std::swap(a, b);
        }
    }

    return checksum_u32(a, (std::size_t)R * C);
}