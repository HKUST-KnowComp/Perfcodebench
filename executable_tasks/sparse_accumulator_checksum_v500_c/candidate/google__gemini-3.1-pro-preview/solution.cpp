#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= FNV_PRIME;
    return hash;
}

// Computes (base^exp) mod 2^64
uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (iters <= 0) return 0;
    if (domain == 0) return FNV_OFFSET_BASIS;

    // The baseline implementation resets 'hash' inside the 'iters' loop,
    // meaning the result of every iteration is identical. We only need to compute it once.
    
    // Strategy: 
    // 1. If domain is small, use a dense array for O(N + D).
    // 2. If domain is large, use sorting and binary exponentiation for O(N log N + N log D).
    
    if (domain < 1000000) {
        std::vector<uint64_t> accum(domain, 0);
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] < domain) {
                accum[indices[i]] += values[i];
            }
        }
        
        uint64_t hash = FNV_OFFSET_BASIS;
        for (uint32_t i = 0; i < domain; ++i) {
            hash = mix(hash, accum[i]);
        }
        return hash;
    } else {
        // Large domain: aggregate sparse updates
        struct Entry {
            uint32_t idx;
            uint32_t val;
        };
        std::vector<Entry> entries;
        entries.reserve(indices.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] < domain) {
                entries.push_back({indices[i], values[i]});
            }
        }
        
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.idx < b.idx;
        });

        uint64_t hash = FNV_OFFSET_BASIS;
        uint32_t current_idx = 0;
        
        size_t i = 0;
        while (i < entries.size()) {
            uint32_t idx = entries[i].idx;
            
            // Skip zeros between current_idx and idx
            if (idx > current_idx) {
                hash *= power(FNV_PRIME, idx - current_idx);
            }
            
            // Accumulate all values for this index
            uint64_t sum = 0;
            while (i < entries.size() && entries[i].idx == idx) {
                sum += entries[i].val;
                i++;
            }
            
            hash = mix(hash, sum);
            current_idx = idx + 1;
        }
        
        // Skip remaining zeros until domain end
        if (current_idx < domain) {
            hash *= power(FNV_PRIME, domain - current_idx);
        }
        
        return hash;
    }
}