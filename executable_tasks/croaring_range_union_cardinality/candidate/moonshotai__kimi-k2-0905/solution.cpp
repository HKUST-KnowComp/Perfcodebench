#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths)
{
    const std::size_t n = starts.size();
    if (n == 0) return 0;

    // Build (start, end) pairs and sort by start
    std::vector<std::pair<uint32_t,uint32_t>> ranges;
    ranges.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t begin = starts[i];
        uint32_t end   = begin + lengths[i];          // exclusive
        ranges.emplace_back(begin, end);
    }
    std::sort(ranges.begin(), ranges.end());

    // Sweep-line merge
    uint64_t union_size = 0;
    uint32_t cur_begin  = ranges[0].first;
    uint32_t cur_end    = ranges[0].second;

    for (std::size_t i = 1; i < n; ++i) {
        uint32_t b = ranges[i].first;
        uint32_t e = ranges[i].second;
        if (b <= cur_end) {
            // overlap or adjacency → merge
            if (e > cur_end) cur_end = e;
        } else {
            // finish current run
            union_size += static_cast<uint64_t>(cur_end) - cur_begin;
            cur_begin   = b;
            cur_end     = e;
        }
    }
    union_size += static_cast<uint64_t>(cur_end) - cur_begin;
    return union_size;
}