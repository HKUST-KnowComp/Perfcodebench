#include "interface.h"

#include <vector>

namespace {

// FNV-1a style hash as per the original checksum_u64
inline uint64_t update_hash(uint64_t hash, uint64_t v) {
    hash ^= v;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters) {
    const size_t n = values.size();
    if (n == 0) return 0; // Handle empty input case

    uint64_t final_hash = 0;
    const size_t win_size = static_cast<size_t>(window);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_window_sum = 0;
        uint64_t hash = 1469598103934665603ULL;

        for (size_t i = 0; i < n; ++i) {
            current_window_sum += values[i];
            if (i >= win_size) {
                current_window_sum -= values[i - win_size];
            }
            hash = update_hash(hash, current_window_sum);
        }
        final_hash = hash;
    }

    return final_hash;
}