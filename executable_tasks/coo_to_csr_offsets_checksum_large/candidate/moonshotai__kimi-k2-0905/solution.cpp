#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // anonymous

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows,
                          int iters)
{
    const uint32_t nnz = static_cast<uint32_t>(row_ids.size());
    const uint32_t off_size = rows + 1;

    // one contiguous buffer reused every iteration
    std::vector<uint32_t> buf(off_size);
    uint32_t* counts = buf.data();

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // 1. zero histogram
        std::memset(counts, 0, off_size * sizeof(uint32_t));

        // 2. build histogram in a single pass over COO rows
        for (uint32_t i = 0; i < nnz; ++i) {
            const uint32_t r = row_ids[i];
            ++counts[r];
        }

        // 3. in-place prefix sum -> CSR offsets
        uint32_t sum = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            const uint32_t cnt = counts[r];
            counts[r] = sum;
            sum += cnt;
        }
        counts[rows] = sum;  // last entry

        // 4. compute FNV-1a hash of the offset array
        // unroll 4-way to reduce loop overhead and hide latency
        const uint32_t* off = counts;
        uint64_t h = 1469598103934665603ULL;
        uint32_t i = 0;
        for (; i + 3 < off_size; i += 4) {
            h = mix(h, off[i]);
            h = mix(h, off[i+1]);
            h = mix(h, off[i+2]);
            h = mix(h, off[i+3]);
        }
        for (; i < off_size; ++i) {
            h = mix(h, off[i]);
        }
        hash = h;
    }
    return hash;
}