#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters)
{
    const std::size_t N = signal.size();
    const std::size_t M = taps.size();

    std::vector<int64_t> output;
    output.reserve(N);

    for (int iter = 0; iter < iters; ++iter) {
        output.clear();

        int64_t window = 0;

        // lead-in: partial convolution
        for (std::size_t i = 0; i < M && i < N; ++i) {
            window += static_cast<int64_t>(signal[i]) * static_cast<int64_t>(taps[i]);
            output.push_back(window);
        }

        // steady state: slide the window
        for (std::size_t i = M; i < N; ++i) {
            window -= static_cast<int64_t>(signal[i - M]) * static_cast<int64_t>(taps[0]);
            // shift taps left by one position
            for (std::size_t k = 0; k < M - 1; ++k) {
                window += static_cast<int64_t>(signal[i - k - 1]) *
                          (static_cast<int64_t>(taps[k + 1]) - static_cast<int64_t>(taps[k]));
            }
            window += static_cast<int64_t>(signal[i]) * static_cast<int64_t>(taps[M - 1]);
            output.push_back(window);
        }

        // checksum
        uint64_t total = 1469598103934665603ULL;
        for (int64_t v : output) {
            uint64_t u = static_cast<uint64_t>(v);
            total ^= (u * u + 3 * u);
            total *= 1099511628211ULL;
        }
        if (iter == iters - 1) return total;
    }
    return 0; // unreachable
}