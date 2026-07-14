#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

struct FastSet {
    std::vector<uint64_t> table;
    uint32_t mask;

    FastSet(const std::vector<uint32_t>& keys) {
        size_t n = keys.size();
        size_t cap = 1;
        while (cap < n * 2) cap <<= 1;
        mask = static_cast<uint32_t>(cap - 1);
        table.assign(cap, 0);
        for (uint32_t k : keys) {
            uint32_t h = k * 0x9e3779b9;
            uint32_t idx = h & mask;
            while (table[idx] != 0) {
                if ((uint32_t)table[idx] == k) break;
                idx = (idx + 1) & mask;
            }
            if (table[idx] == 0) {
                table[idx] = (1ULL << 32) | k;
            }
        }
    }

    inline bool contains(uint32_t k) const {
        uint32_t h = k * 0x9e3779b9;
        uint32_t idx = h & mask;
        while (true) {
            uint64_t val = table[idx];
            if (val == 0) return false;
            if ((uint32_t)val == k) return true;
            idx = (idx + 1) & mask;
        }
    }
};

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
    (void)hash_space;
    FastSet cache(sorted_cache_keys);
    uint64_t hash = 0;
    const uint32_t* blocks_ptr = request_blocks.data();
    const int* offsets_ptr = request_offsets.data();
    size_t num_offsets = request_offsets.size();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t r = 0; r + 1 < num_offsets; ++r) {
            uint64_t prefix = 0;
            int start = offsets_ptr[r];
            int end = offsets_ptr[r + 1];
            for (int i = start; i < end; ++i) {
                if (!cache.contains(blocks_ptr[i])) break;
                ++prefix;
            }
            hash ^= prefix;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
