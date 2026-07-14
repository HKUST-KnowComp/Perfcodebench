#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    if (iters <= 0) return 0;

    const uint32_t* b = boundaries.data();
    const std::size_t B = boundaries.size();
    const uint32_t* p = probes.data();
    const std::size_t P = probes.size();

    std::size_t bi = 0;
    uint64_t sum = 0;

    for (std::size_t i = 0; i < P; ++i) {
        uint32_t v = p[i];
        while (bi < B && b[bi] <= v) {
            ++bi;
        }
        sum += static_cast<uint64_t>(bi);
    }

    return sum;
}
