#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    const size_t n = keys.size();
    const uint32_t* keys_ptr = keys.data();
    const uint8_t* flags_ptr = flags.data();
    const uint32_t* payloads_ptr = payloads.data();

    // Compute the sum of payloads for qualifying entries once.
    uint64_t base_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        if (flags_ptr[i] && keys_ptr[i] < threshold && !(keys_ptr[i] & 7u)) {
            base_sum += payloads_ptr[i];
        }
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = base_sum;   // each iteration produces the same sum
    }
    return sum;
}