#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    (void)iters;

    size_t N = signal.size();
    size_t M = taps.size();
    std::vector<int64_t> output(N, 0);

    const int16_t* sig = signal.data();
    int64_t* out = output.data();

    for (size_t j = 0; j < M; ++j) {
        int64_t t = taps[j];
        const int16_t* s = sig;
        int64_t* o = out + j;
        size_t len = N - j;
        for (size_t k = 0; k < len; ++k) {
            o[k] += static_cast<int64_t>(s[k]) * t;
        }
    }

    uint64_t total = 1469598103934665603ULL;
    for (size_t i = 0; i < N; ++i) {
        int64_t acc = output[i];
        total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
        total *= 1099511628211ULL;
    }

    return total;
}