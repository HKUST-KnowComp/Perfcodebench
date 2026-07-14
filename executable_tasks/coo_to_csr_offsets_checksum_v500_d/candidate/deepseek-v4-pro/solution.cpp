#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (iters <= 0) {
        return 0;
    }

    std::vector<uint32_t> offsets(rows + 1);
    const uint32_t* data = row_ids.data();
    size_t N = row_ids.size();
    size_t idx = 0;
    uint32_t cumulative = 0;
    offsets[0] = 0;

    for (uint32_t r = 0; r < rows; ++r) {
        uint32_t count = 0;
        while (idx < N && data[idx] == r) {
            ++count;
            ++idx;
        }
        cumulative += count;
        offsets[r + 1] = cumulative;
    }
    // Any remaining row_ids with row >= rows are ignored (original behavior).

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash = mix(hash, v);
    }
    return hash;
}