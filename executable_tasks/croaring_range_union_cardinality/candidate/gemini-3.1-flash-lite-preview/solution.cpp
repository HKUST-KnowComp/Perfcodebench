#include "interface.h"
#include <algorithm>
#include <vector>

struct Interval {
    uint32_t start;
    uint32_t end;
};

uint64_t union_cardinality(const std::vector<uint32_t>& starts, const std::vector<uint32_t>& lengths) {
    if (starts.empty()) return 0;

    size_t n = starts.size();
    std::vector<Interval> intervals(n);
    for (size_t i = 0; i < n; ++i) {
        intervals[i] = {starts[i], starts[i] + lengths[i]};
    }

    std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
        return a.start < b.start;
    });

    uint64_t total_cardinality = 0;
    if (n == 0) return 0;

    uint32_t current_start = intervals[0].start;
    uint32_t current_end = intervals[0].end;

    for (size_t i = 1; i < n; ++i) {
        if (intervals[i].start < current_end) {
            if (intervals[i].end > current_end) {
                current_end = intervals[i].end;
            }
        } else {
            total_cardinality += (uint64_t)current_end - current_start;
            current_start = intervals[i].start;
            current_end = intervals[i].end;
        }
    }
    total_cardinality += (uint64_t)current_end - current_start;

    return total_cardinality;
}