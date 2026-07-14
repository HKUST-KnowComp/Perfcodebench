#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters)
{
    const int N = static_cast<int>(signal.size());
    const int K = static_cast<int>(taps.size());

    std::vector<int64_t> output(N);

    uint64_t total = 1469598103934665603ULL;
    const uint64_t M = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // --- convolution ---
        for (int i = 0; i < N; ++i) {
            const int16_t* s_ptr = &signal[i];
            const int16_t* t_ptr = &taps[0];
            int64_t acc = 0;

            const int limit = (i < K) ? i + 1 : K;
            int j = 0;

            // 8-way unroll
            for (; j + 7 < limit; j += 8) {
                acc += static_cast<int64_t>(s_ptr[-j]) * static_cast<int64_t>(t_ptr[j]);
                acc += static_cast<int64_t>(s_ptr[-j-1]) * static_cast<int64_t>(t_ptr[j+1]);
                acc += static_cast<int64_t>(s_ptr[-j-2]) * static_cast<int64_t>(t_ptr[j+2]);
                acc += static_cast<int64_t>(s_ptr[-j-3]) * static_cast<int64_t>(t_ptr[j+3]);
                acc += static_cast<int64_t>(s_ptr[-j-4]) * static_cast<int64_t>(t_ptr[j+4]);
                acc += static_cast<int64_t>(s_ptr[-j-5]) * static_cast<int64_t>(t_ptr[j+5]);
                acc += static_cast<int64_t>(s_ptr[-j-6]) * static_cast<int64_t>(t_ptr[j+6]);
                acc += static_cast<int64_t>(s_ptr[-j-7]) * static_cast<int64_t>(t_ptr[j+7]);
            }
            // tail
            for (; j < limit; ++j)
                acc += static_cast<int64_t>(s_ptr[-j]) * static_cast<int64_t>(t_ptr[j]);

            output[i] = acc;
        }

        // --- checksum ---
        total = 1469598103934665603ULL;
        const int64_t* out = output.data();
        int i = 0;
        for (; i + 7 < N; i += 8) {
            uint64_t v0 = static_cast<uint64_t>(out[i+0] * out[i+0] + 3 * out[i+0]);
            uint64_t v1 = static_cast<uint64_t>(out[i+1] * out[i+1] + 3 * out[i+1]);
            uint64_t v2 = static_cast<uint64_t>(out[i+2] * out[i+2] + 3 * out[i+2]);
            uint64_t v3 = static_cast<uint64_t>(out[i+3] * out[i+3] + 3 * out[i+3]);
            uint64_t v4 = static_cast<uint64_t>(out[i+4] * out[i+4] + 3 * out[i+4]);
            uint64_t v5 = static_cast<uint64_t>(out[i+5] * out[i+5] + 3 * out[i+5]);
            uint64_t v6 = static_cast<uint64_t>(out[i+6] * out[i+6] + 3 * out[i+6]);
            uint64_t v7 = static_cast<uint64_t>(out[i+7] * out[i+7] + 3 * out[i+7]);

            total ^= v0; total *= M;
            total ^= v1; total *= M;
            total ^= v2; total *= M;
            total ^= v3; total *= M;
            total ^= v4; total *= M;
            total ^= v5; total *= M;
            total ^= v6; total *= M;
            total ^= v7; total *= M;
        }
        for (; i < N; ++i) {
            uint64_t v = static_cast<uint64_t>(out[i] * out[i] + 3 * out[i]);
            total ^= v;
            total *= M;
        }
    }
    return total;
}