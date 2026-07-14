#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    if (iters <= 0) {
        return 0;
    }
    uint64_t sum = 0;
    const size_t bsz = boundaries.size();
    const uint32_t* bdata = boundaries.data();
    const uint32_t* pdata = probes.data();
    const size_t psz = probes.size();
    size_t bidx = 0;
    for (size_t i = 0; i < psz; ++i) {
        uint32_t probe = pdata[i];
        while (bidx < bsz && bdata[bidx] <= probe) {
            ++bidx;
        }
        sum += static_cast<uint64_t>(bidx);
    }
    return sum;
}
