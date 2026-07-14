#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters)
{
    const std::size_t n = values.size();
    if (n == 0) return 0;

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        uint32_t prev = 0;
        std::size_t i = 0;

        // 4-way unrolled portion
        for (; i + 3 < n; i += 4) {
            uint32_t re0 = run_ends[i];
            uint32_t re1 = run_ends[i+1];
            uint32_t re2 = run_ends[i+2];
            uint32_t re3 = run_ends[i+3];

            uint32_t cnt0 = re0 - prev;                    local += static_cast<uint64_t>(values[i])   * cnt0;
            uint32_t cnt1 = re1 - re0;                      local += static_cast<uint64_t>(values[i+1]) * cnt1;
            uint32_t cnt2 = re2 - re1;                      local += static_cast<uint64_t>(values[i+2]) * cnt2;
            uint32_t cnt3 = re3 - re2;                      local += static_cast<uint64_t>(values[i+3]) * cnt3;

            prev = re3;
        }
        // scalar tail
        for (; i < n; ++i) {
            uint32_t cnt = run_ends[i] - prev;
            local += static_cast<uint64_t>(values[i]) * cnt;
            prev = run_ends[i];
        }
        sum = local;
    }
    return sum;
}