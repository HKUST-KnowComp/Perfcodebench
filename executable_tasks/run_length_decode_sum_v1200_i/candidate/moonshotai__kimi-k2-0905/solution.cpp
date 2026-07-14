#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters)
{
    uint64_t total = 0;
    const std::size_t n = values.size();
    for (std::size_t i = 0; i < n; ++i) {
        const uint64_t v = values[i];
        const uint64_t l = lengths[i];
        total += v * l;
    }
    return total * static_cast<uint64_t>(iters);
}