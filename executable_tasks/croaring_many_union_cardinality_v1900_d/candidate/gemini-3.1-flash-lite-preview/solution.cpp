#include "interface.h"
#include <queue>
#include <vector>

struct Iterator {
    const uint32_t* ptr;
    const uint32_t* end;

    bool operator>(const Iterator& other) const {
        return *ptr > *other.ptr;
    }
};

uint64_t many_union_cardinality(const std::vector<uint32_t>& offsets, const std::vector<uint32_t>& values) {
    if (offsets.size() < 2) return 0;

    std::priority_queue<Iterator, std::vector<Iterator>, std::greater<Iterator>> pq;

    for (size_t i = 0; i + 1 < offsets.size(); ++i) {
        if (offsets[i] < offsets[i + 1]) {
            pq.push({&values[offsets[i]], &values[offsets[i + 1]]});
        }
    }

    uint64_t count = 0;
    uint32_t last_val = 0;
    bool first = true;

    while (!pq.empty()) {
        Iterator top = pq.top();
        pq.pop();

        uint32_t current_val = *top.ptr;
        if (first || current_val != last_val) {
            count++;
            last_val = current_val;
            first = false;
        }

        if (top.ptr + 1 < top.end) {
            top.ptr++;
            pq.push(top);
        }
    }

    return count;
}