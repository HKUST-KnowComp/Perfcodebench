#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Simple fast hash set for uint32_t
struct FastSet {
    std::vector<uint32_t> table;
    uint32_t mask;
    static constexpr uint32_t EMPTY = 0xFFFFFFFF;

    FastSet(const std::vector<uint32_t>& keys) {
        uint32_t size = 1;
        while (size < keys.size() * 2) size <<= 1;
        table.assign(size, EMPTY);
        mask = size - 1;
        for (uint32_t k : keys) {
            uint32_t pos = hash_fn(k) & mask;
            while (table[pos] != EMPTY) {
                pos = (pos + 1) & mask;
            }
            table[pos] = k;
        }
    }

    bool contains(uint32_t k) const {
        uint32_t pos = hash_fn(k) & mask;
        while (table[pos] != EMPTY) {
            if (table[pos] == k) return true;
            pos = (pos + 1) & mask;
        }
        return false;
    }

    static uint32_t hash_fn(uint32_t x) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
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

    // Pre-build a fast lookup table for the cache keys
    FastSet cache_set(sorted_cache_keys);

    uint64_t final_hash = 0;
    const size_t num_requests = request_offsets.size() - 1;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_requests; ++r) {
            uint64_t prefix = 0;
            const int start = request_offsets[r];
            const int end = request_offsets[r + 1];
            for (int i = start; i < end; ++i) {
                if (!cache_set.contains(request_blocks[i])) {
                    break;
                }
                ++prefix;
            }
            current_hash = mix(current_hash, prefix);
        }
        // The original code resets hash = 1469... inside the loop and returns the last iter's result
        // but the baseline logic actually returns the hash of the LAST iteration.
        final_hash = current_hash;
    }
    return final_hash;
}