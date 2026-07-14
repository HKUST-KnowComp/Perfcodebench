#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t h, uint64_t x) {
    h ^= x;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows, int iters) {
    std::vector<uint32_t> cnt(rows, 0);
    std::vector<uint32_t> off(rows + 1);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // reset counts
        std::fill(cnt.begin(), cnt.end(), 0);

        // counting pass – row_ids is sorted, so this is cache-friendly
        for (uint32_t r : row_ids) {
            ++cnt[r];
        }

        // prefix sum into offsets
        uint32_t sum = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            off[r] = sum;
            sum += cnt[r];
        }
        off[rows] = sum;

        // hash the offset array
        hash = 1469598103934665603ULL;
        const uint32_t* ptr = off.data();
        const uint32_t* end = ptr + off.size();
        while (ptr + 4 <= end) {
            hash = mix(hash, ptr[0]);
            hash = mix(hash, ptr[1]);
            hash = mix(hash, ptr[2]);
            hash = mix(hash, ptr[3]);
            ptr += 4;
        }
        while (ptr != end) {
            hash = mix(hash, *ptr++);
        }
    }
    return hash;
}