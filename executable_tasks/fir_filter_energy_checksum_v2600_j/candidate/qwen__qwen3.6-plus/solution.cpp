#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
    
    const size_t N = signal.size();
    const size_t M = taps.size();
    const int16_t* sig = signal.data();
    
    // Pre-reverse taps for efficient forward dot-product in steady state
    std::vector<int16_t> taps_rev(taps.rbegin(), taps.rend());
    const int16_t* tap_r = taps_rev.data();
    
    uint64_t total = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        total = 1469598103934665603ULL;
        
        // Ramp-up phase: i < M
        size_t ramp_end = (M < N) ? M : N;
        for (size_t i = 0; i < ramp_end; ++i) {
            int64_t acc = 0;
            for (size_t j = 0; j <= i; ++j) {
                acc += static_cast<int64_t>(sig[i - j]) * taps[j];
            }
            total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
            total *= 1099511628211ULL;
        }
        
        // Steady-state phase: i >= M
        for (size_t i = M; i < N; ++i) {
            int64_t acc = 0;
            const int16_t* s = &sig[i - M + 1];
            for (size_t j = 0; j < M; ++j) {
                acc += static_cast<int64_t>(s[j]) * tap_r[j];
            }
            total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
            total *= 1099511628211ULL;
        }
    }
    return total;
}