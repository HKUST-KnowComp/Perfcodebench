#include "interface.h"

#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
    const uint32_t* v = values.data();
    const uint16_t* l = lengths.data();
    const size_t n = values.size();
    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;
        for (; i + 4 <= n; i += 4) {
            sum += static_cast<uint64_t>(v[i])   * l[i];
            sum += static_cast<uint64_t>(v[i+1]) * l[i+1];
            sum += static_cast<uint64_t>(v[i+2]) * l[i+2];
            sum += static_cast<uint64_t>(v[i+3]) * l[i+3];
        }
        for (; i < n; ++i) {
            sum += static_cast<uint64_t>(v[i]) * l[i];
        }
        total_sum = sum;
    }
    return total_sum;
}
