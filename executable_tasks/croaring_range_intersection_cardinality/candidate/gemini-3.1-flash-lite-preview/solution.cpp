#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

struct Interval {
    uint32_t start;
    uint32_t end;
};

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {

    auto to_intervals = [](const std::vector<uint32_t>& starts, const std::vector<uint32_t>& lengths) {
        std::vector<Interval> intervals;
        intervals.reserve(starts.size());
        for (size_t i = 0; i < starts.size(); ++i) {
            intervals.push_back({starts[i], starts[i] + lengths[i]});
        }
        std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
            return a.start < b.start;
        });
        return intervals;
    };

    std::vector<Interval> lhs = to_intervals(lhs_starts, lhs_lengths);
    std::vector<Interval> rhs = to_intervals(rhs_starts, rhs_lengths);

    uint64_t total_intersection = 0;
    size_t i = 0, j = 0;

    while (i < lhs.size() && j < rhs.size()) {
        uint32_t start = std::max(lhs[i].start, rhs[j].start);
        uint32_t end = std::min(lhs[i].end, rhs[j].end);

        if (start < end) {
            total_intersection += (uint64_t)(end - start);
        }

        if (lhs[i].end < rhs[j].end) {
            i++;
        } else {
            j++;
        }
    }

    return total_intersection;
}