#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // unnamed

uint64_t segmented_reduce_checksum(
    const std::vector<uint32_t>& segment_ids,
    const std::vector<uint32_t>& values,
    uint32_t segments,
    int iters)
{
    const std::size_t segs = static_cast<std::size_t>(segments);
    std::vector<uint32_t> offsets(segs + 1, 0);
    std::vector<uint64_t> sums(segs, 0);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(offsets.begin(), offsets.end(), 0);
        std::fill(sums.begin(), sums.end(), 0);

        uint32_t cur_id = 0;
        uint64_t cur_sum = 0;

        const std::size_t n = segment_ids.size();
        for (std::size_t i = 0; i < n; ) {
            const uint32_t id = segment_ids[i];
            while (i < n && segment_ids[i] == id) {
                cur_sum += static_cast<uint64_t>(values[i]);
                ++i;
            }
            sums[id] = cur_sum;
            offsets[id + 1] = static_cast<uint32_t>(i);
            cur_sum = 0;
        }

        hash = 1469598103934665603ULL;
        for (std::size_t s = 0; s <= segs; ++s)
            hash = mix(hash, offsets[s]);
        for (std::size_t s = 0; s < segs; ++s)
            hash = mix(hash, sums[s]);
    }
    return hash;
}