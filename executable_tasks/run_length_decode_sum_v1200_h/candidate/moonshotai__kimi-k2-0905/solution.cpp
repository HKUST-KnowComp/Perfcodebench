#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters)
{
    const std::size_t n = values.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            sum += static_cast<uint64_t>(values[i]) * static_cast<uint64_t>(lengths[i]);
        }
    }
    return sum;
}