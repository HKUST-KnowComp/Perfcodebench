#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();
    const std::size_t n = probe_keys.size();

    const uint64_t* dv = reinterpret_cast<const uint64_t*>(dim_values.data());

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        std::size_t i = 0;

        for (; i + 3 < n; i += 4) {
            const uint32_t k0 = pk[i];
            const uint32_t k1 = pk[i + 1];
            const uint32_t k2 = pk[i + 2];
            const uint32_t k3 = pk[i + 3];

            s0 += dv[k0] * pw[i];
            s1 += dv[k1] * pw[i + 1];
            s2 += dv[k2] * pw[i + 2];
            s3 += dv[k3] * pw[i + 3];
        }
        for (; i < n; ++i) {
            s0 += dv[pk[i]] * pw[i];
        }
        total = s0 + s1 + s2 + s3;
    }

    return total;
}