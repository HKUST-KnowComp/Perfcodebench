#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    const size_t N = signal.size();
    const size_t M = taps.size();
    uint64_t final_total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t total = 1469598103934665603ULL;
        const size_t first_phase_end = std::min(M, N);

        // Transient phase: partial filter overlap, no internal branches
        for (size_t i = 0; i < first_phase_end; ++i) {
            int64_t acc = 0;
            for (size_t j = 0; j <= i; ++j) {
                acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
            }
            // Update checksum immediately, no need to store accumulator
            total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
            total *= 1099511628211ULL;
        }

        // Steady-state phase: full filter overlap, no bounds checks required
        for (size_t i = M; i < N; ++i) {
            int64_t acc = 0;
            for (size_t j = 0; j < M; ++j) {
                acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
            }
            total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
            total *= 1099511628211ULL;
        }

        final_total = total;
    }

    return final_total;
}