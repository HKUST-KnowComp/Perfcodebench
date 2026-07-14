#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // anonymous namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows,
                          int iters)
{
    const uint32_t rows_p1 = rows + 1;
    std::vector<uint32_t> hist(rows, 0);
    std::vector<uint32_t> offsets(rows_p1, 0);

    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // clear histogram
        std::fill(hist.begin(), hist.end(), 0u);

        // build histogram from sorted COO rows
        for (uint32_t r : row_ids)
            ++hist[r];

        // prefix sum -> CSR offsets
        uint32_t cum = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            offsets[r] = cum;
            cum += hist[r];
        }
        offsets[rows] = cum;

        // compute checksum
        hash = 14695981039346656037ULL;
        for (uint32_t v : offsets)
            hash = mix(hash, v);
    }
    return hash;
}