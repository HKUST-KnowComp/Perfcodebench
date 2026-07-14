#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    (void)iters;
    
    const size_t N = signal.size();
    if (N == 0) return 1469598103934665603ULL;
    
    const size_t M = taps.size();
    const int16_t* sig = signal.data();
    const int16_t* tap = taps.data();
    
    uint64_t total = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    size_t limit = (M < N) ? M : N;
    
    for (size_t i = 0; i < limit; ++i) {
        int64_t acc = 0;
        for (size_t j = 0; j <= i; ++j) {
            acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tap[j]);
        }
        uint64_t val = static_cast<uint64_t>(acc * acc + 3 * acc);
        total ^= val;
        total *= fnv_prime;
    }
    
    for (size_t i = limit; i < N; ++i) {
        int64_t acc = 0;
        for (size_t j = 0; j < M; ++j) {
            acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tap[j]);
        }
        uint64_t val = static_cast<uint64_t>(acc * acc + 3 * acc);
        total ^= val;
        total *= fnv_prime;
    }
    
    return total;
}