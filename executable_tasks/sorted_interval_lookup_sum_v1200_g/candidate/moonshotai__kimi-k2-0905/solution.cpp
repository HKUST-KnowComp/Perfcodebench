#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const std::size_t P = probes.size();
    const std::size_t N = starts.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0;          // cursor over intervals
        uint64_t sum = 0;
        for (std::size_t j = 0; j < P; ++j) {
            const uint32_t probe = probes[j];
            // advance interval cursor to the last start <= probe
            while (i + 1 < N && starts[i + 1] <= probe) ++i;
            // check membership: start[i] <= probe <= end[i]
            if (probe >= starts[i] && probe <= ends[i]) {
                sum += static_cast<uint64_t>(payloads[i]) *
                       static_cast<uint64_t>(weights[j]);
            }
        }
        total = sum;
    }
    return total;
}