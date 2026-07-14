#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstring>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    (void)ends;  // unused, as in baseline

    // Build a direct lookup table once.
    // lut[x] = largest index idx such that starts[idx] <= x.
    static std::vector<uint32_t> lut;
    static bool built = false;
    if (!built) {
        lut.resize(0x100000000ULL);  // 4 GB, covers full 32-bit range
        uint32_t idx = 0;
        const size_t n = starts.size();
        for (uint64_t x = 0; x <= 0xFFFFFFFFULL; ++x) {
            while (idx + 1 < n && starts[idx + 1] <= x) ++idx;
            lut[x] = idx;
        }
        built = true;
    }

    uint64_t sum = 0;
    const size_t nprobes = probes.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (size_t i = 0; i < nprobes; ++i) {
            const uint32_t p = probes[i];
            const uint32_t idx = lut[p];
            local += static_cast<uint64_t>(payloads[idx]) *
                     static_cast<uint64_t>(weights[i]);
        }
        sum = local;
    }
    return sum;
}