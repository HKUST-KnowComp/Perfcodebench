#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    const std::size_t signal_len = signal.size();
    const std::size_t taps_len = taps.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        total = 1469598103934665603ULL;
        // Process initial transient (i < tap count) to avoid runtime bounds checks
        const std::size_t first_pass_limit = (taps_len < signal_len) ? taps_len : signal_len;
        for (std::size_t i = 0; i < first_pass_limit; ++i) {
            int64_t acc = 0;
            for (std::size_t j = 0; j <= i; ++j) {
                acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
            }
            // Update checksum immediately, no intermediate output storage needed
            const uint64_t acc_u = static_cast<uint64_t>(acc);
            total ^= acc_u * acc_u + 3 * acc_u;
            total *= 1099511628211ULL;
        }
        // Process steady state where all taps are valid, no bounds checks required
        for (std::size_t i = taps_len; i < signal_len; ++i) {
            int64_t acc = 0;
            for (std::size_t j = 0; j < taps_len; ++j) {
                acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
            }
            const uint64_t acc_u = static_cast<uint64_t>(acc);
            total ^= acc_u * acc_u + 3 * acc_u;
            total *= 1099511628211ULL;
        }
    }
    return total;
}