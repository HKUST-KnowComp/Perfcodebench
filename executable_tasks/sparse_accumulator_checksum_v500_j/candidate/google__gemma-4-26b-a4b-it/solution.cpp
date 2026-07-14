#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

struct Entry {
    uint32_t idx;
    uint32_t val;
};

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    if (indices.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        for (int iter = 0; iter < iters; ++iter) {
            for (uint32_t k = 0; k < domain; ++k) {
                hash = mix(hash, 0ULL);
            }
        }
        return hash;
    }

    // Pre-sort indices to allow linear accumulation
    std::vector<Entry> entries(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        entries[i] = {indices[i], values[i]};
    }
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.idx < b.idx;
    });

    // Pre-aggregate values for the same index
    std::vector<std::pair<uint32_t, uint64_t>> aggregated;
    if (!entries.empty()) {
        uint32_t current_idx = entries[0].idx;
        uint64_t current_sum = entries[0].val;
        for (size_t i = 1; i < entries.size(); ++i) {
            if (entries[i].idx == current_idx) {
                current_sum += entries[i].val;
            } else {
                aggregated.push_back({current_idx, current_sum});
                current_idx = entries[i].idx;
                current_sum = entries[i].val;
            }
        }
        aggregated.push_back({current_idx, current_sum});
    }

    uint64_t hash = 1469598103934665603ULL;
    
    // The baseline logic: for each iteration, the accumulator is cleared.
    // This means the hash is recalculated from the same starting constant every iteration.
    // However, the hash is NOT reset to the constant inside the 'iter' loop in the baseline?
    // Let's re-read: 
    // for (int iter = 0; iter < iters; ++iter) {
    //    accum.clear();
    //    ... populate accum ...
    //    hash = 1469598103934665603ULL;
    //    for (uint32_t key = 0; key < domain; ++key) { ... hash = mix(hash, ...) }
    // }
    // This means the 'hash' value from the previous iteration is overwritten by the constant.
    // Therefore, the 'iters' loop is actually redundant if iters > 0, as it just repeats the same calculation.
    // The final 'hash' is simply the result of one iteration.

    if (iters > 0) {
        hash = 1469598103934665603ULL;
        size_t agg_ptr = 0;
        for (uint32_t k = 0; k < domain; ++k) {
            uint64_t val = 0;
            if (agg_ptr < aggregated.size() && aggregated[agg_ptr].first == k) {
                val = aggregated[agg_ptr].second;
                agg_ptr++;
            }
            hash = mix(hash, val);
        }
    }

    return hash;
}