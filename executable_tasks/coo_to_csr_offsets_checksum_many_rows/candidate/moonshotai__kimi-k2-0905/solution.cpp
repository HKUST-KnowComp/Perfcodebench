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

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    std::vector<uint32_t> hist(rows, 0u);
    const uint32_t nnz = static_cast<uint32_t>(row_ids.size());

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // clear histogram
        std::fill(hist.begin(), hist.end(), 0u);

        // build histogram
        for (uint32_t i = 0; i < nnz; ++i) {
            ++hist[row_ids[i]];
        }

        // prefix-sum + checksum
        uint32_t off = 0;
        hash = 1469598103934665603ULL;
        hash = mix(hash, off);               // offsets[0] == 0
        for (uint32_t r = 0; r < rows; ++r) {
            off += hist[r];
            hash = mix(hash, static_cast<uint64_t>(off));
        }
    }
    return hash;
}