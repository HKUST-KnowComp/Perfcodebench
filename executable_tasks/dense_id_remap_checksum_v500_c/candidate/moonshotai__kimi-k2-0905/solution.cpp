#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}
} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters)
{
    if (keys.empty() || probes.empty() || iters <= 0) return 0;

    const uint32_t max_key = keys.back();
    std::vector<uint32_t> map(max_key + 1, 0);

    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    for (size_t i = 0, n = keys.size(); i < n; ++i) {
        map[k[i]] = v[i];
    }

    const uint32_t* p = probes.data();
    const size_t probe_cnt = probes.size();

    uint64_t h = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t j = 0; j < probe_cnt; ++j) {
            h = mix(h, map[p[j]]);
        }
    }
    return h;
}