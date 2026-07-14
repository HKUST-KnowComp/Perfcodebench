#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    if (iters <= 0) return 0;

    const int16_t* sig = signal.data();
    const int16_t* tap = taps.data();
    const size_t n = signal.size();
    const size_t m = taps.size();

    uint64_t total = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < n; ++i) {
        size_t len = (i < m) ? (i + 1) : m;
        int64_t acc = 0;
        for (size_t j = 0; j < len; ++j) {
            acc += static_cast<int64_t>(sig[i - j]) * tap[j];
        }
        uint64_t uacc = static_cast<uint64_t>(acc);
        total ^= uacc * uacc + 3 * uacc;
        total *= fnv_prime;
    }
    return total;
}
