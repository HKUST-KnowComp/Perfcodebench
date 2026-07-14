#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    // Build a dense bitset once, reuse every iteration
    const uint32_t words = (key_space + 63) / 64;
    std::vector<uint64_t> bitmap(words, 0);
    for (uint32_t k : enabled_keys) {
        if (k < key_space) {
            bitmap[k >> 6] |= 1ULL << (k & 63);
        }
    }

    uint64_t sum = 0;
    const std::size_t n = probe_keys.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t key = probe_keys[i];
            if ((bitmap[key >> 6] >> (key & 63)) & 1) {
                local_sum += static_cast<uint64_t>(values[i]);
            }
        }
        sum = local_sum;  // final iteration value is returned
    }
    return sum;
}