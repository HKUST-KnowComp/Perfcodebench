#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal, 
    const std::vector<int16_t>& taps, 
    int iters) {
    const size_t n = signal.size();
    const size_t m = taps.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        total = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            int64_t acc = 0;
            size_t limit = (i < m) ? (i + 1) : m;
            const int16_t* s_ptr = &signal[i];
            const int16_t* t_ptr = &taps[0];
            
            for (size_t j = 0; j < limit; ++j) {
                acc += static_cast<int64_t>(s_ptr[-static_cast<int>(j)]) * static_cast<int64_t>(t_ptr[j]);
            }
            
            total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
            total *= 1099511628211ULL;
        }
    }
    return total;
}