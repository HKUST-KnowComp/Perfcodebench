#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    (void)ends;  // unused in the problem contract

    const size_t n = probes.size();
    const size_t m = starts.size();

    // pair probe-idx -> weight so we can reorder both arrays together while keeping correspondence
    struct Pw { uint32_t probe; uint32_t weight; };
    std::vector<Pw> pw(n);
    for (size_t i = 0; i < n; ++i) {
        pw[i].probe = probes[i];
        pw[i].weight = weights[i];
    }
    // sort pairs ascending by probe
    std::sort(pw.begin(), pw.end(), [](const Pw& a, const Pw& b){ return a.probe < b.probe; });

    // reusable arrays: probes sorted, weights in same order
    std::vector<uint32_t> sprobes(n);
    std::vector<uint32_t> sweights(n);
    for (size_t i = 0; i < n; ++i) {
        sprobes[i] = pw[i].probe;
        sweights[i] = pw[i].weight;
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;      // index into intervals
        size_t j = 0;      // index into sprobes/sweights
        while (j < n) {
            uint32_t p = sprobes[j];
            // advance to the interval whose start <= p (intervals are non-overlapping and sorted, so exact hit)
            while (i < m && starts[i] > p) ++i;
            // accumulate all probes that fall into this same interval without branching inside
            size_t jfirst = j;
            while (j < n && sprobes[j] < starts[i+1]) {
                sum += uint64_t(payloads[i]) * uint64_t(sweights[j]);
                ++j;
            }
        }
    }

    return sum;
}