#include "interface.h"

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    const uint32_t* data = row_ids.data();
    const std::size_t n = row_ids.size();
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        offsets[0] = 0;
        uint32_t cur = 0;
        std::size_t idx = 0;
        while (cur < rows) {
            uint32_t count = 0;
            while (idx < n && data[idx] == cur) {
                ++count;
                ++idx;
            }
            offsets[static_cast<std::size_t>(cur + 1u)] = offsets[static_cast<std::size_t>(cur)] + count;
            ++cur;
        }
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < static_cast<std::size_t>(rows) + 1u; ++i) {
            hash ^= offsets[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
