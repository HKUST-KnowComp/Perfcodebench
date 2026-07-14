#include "interface.h"
#include <vector>
#include <cstring>
#include <cstdint>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> offsets(rows + 1);
    uint64_t hash = 0;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(offsets.data(), 0, (rows + 1) * sizeof(uint32_t));

        for (uint32_t r : row_ids) {
            offsets[r + 1]++;
        }

        hash = FNV_OFFSET;
        hash *= FNV_PRIME; // Process offsets[0] which is always 0

        uint32_t acc = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            acc += offsets[r + 1];
            hash ^= acc;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}