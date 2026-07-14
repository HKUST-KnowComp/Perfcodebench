#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {

    struct Endpoint {
        uint32_t pos;
        uint8_t  type;   // 0: lhs open, 1: lhs close, 2: rhs open, 3: rhs close
    };

    const size_t n = lhs_starts.size() + rhs_starts.size();
    std::vector<Endpoint> ev;
    ev.reserve(2 * n);

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t b = lhs_starts[i];
        uint32_t e = b + lhs_lengths[i];          // exclusive
        ev.push_back({b, 0});
        ev.push_back({e, 1});
    }
    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t b = rhs_starts[i];
        uint32_t e = b + rhs_lengths[i];
        ev.push_back({b, 2});
        ev.push_back({e, 3});
    }

    auto cmp = [](const Endpoint& a, const Endpoint& b) {
        return a.pos < b.pos || (a.pos == b.pos && a.type < b.type);
    };
    std::sort(ev.begin(), ev.end(), cmp);

    uint64_t count = 0;
    uint32_t lhs_active = 0;
    uint32_t rhs_active = 0;
    uint32_t prev = 0;

    for (const auto& ep : ev) {
        uint32_t cur = ep.pos;
        if (lhs_active && rhs_active && cur > prev) {
            uint64_t seg = uint64_t(cur) - uint64_t(prev);
            count += seg;
        }
        switch (ep.type) {
            case 0: ++lhs_active; break;
            case 1: --lhs_active; break;
            case 2: ++rhs_active; break;
            case 3: --rhs_active; break;
        }
        prev = cur;
    }
    return count;
}