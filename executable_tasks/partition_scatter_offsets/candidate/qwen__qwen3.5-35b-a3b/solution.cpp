#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
    std::vector<uint32_t> counts(groups);
    std::vector<uint32_t> offsets(groups + 1);
    std::vector<uint32_t> rows(keys.size());
    std::vector<uint32_t> current_offsets(groups + 1);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(counts.begin(), counts.end(), 0);
        for (uint32_t key : keys) {
            counts[key]++;
        }

        offsets[0] = 0;
        for (uint32_t g = 0; g < groups; ++g) {
            offsets[g + 1] = offsets[g] + counts[g];
        }

        std::copy(offsets.begin(), offsets.end(), current_offsets.begin());

        for (uint32_t i = 0; i < keys.size(); ++i) {
            uint32_t key = keys[i];
            rows[current_offsets[key]++] = i;
        }

        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash = mix(hash, v);
        }
        for (uint32_t v : rows) {
            hash = mix(hash, v);
        }
    }

    return hash;
}