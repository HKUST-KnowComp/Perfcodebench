#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

struct Entry {
    uint32_t index;
    uint64_t value;
};

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    
    if (indices.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t k = 0; k < domain; ++k) {
            hash = mix(hash, 0ULL);
        }
        // Note: The loop above is still O(domain). 
        // However, mathematically, mixing 0 repeatedly can be optimized.
        // But for correctness with the specific mix function:
        for (int i = 0; i < iters; ++i) {
            hash = 1469598103934665603ULL;
            for (uint32_t k = 0; k < domain; ++k) {
                hash = mix(hash, 0ULL);
            }
        }
        return hash;
    }

    // Pre-allocate to avoid reallocations
    std::vector<Entry> entries(indices.size());
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (std::size_t i = 0; i < indices.size(); ++i) {
            entries[i] = {indices[i], static_cast<uint64_t>(values[i])};
        }

        // Sort by index to allow linear accumulation and gap detection
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.index < b.index;
        });

        // Accumulate values for duplicate indices
        std::vector<Entry> compressed;
        compressed.reserve(entries.size());
        if (!entries.empty()) {
            compressed.push_back(entries[0]);
            for (std::size_t i = 1; i < entries.size(); ++i) {
                if (entries[i].index == compressed.back().index) {
                    compressed.back().value += entries[i].value;
                } else {
                    compressed.push_back(entries[i]);
                }
            }
        }

        uint64_t hash = 1469598103934665603ULL;
        uint32_t current_idx = 0;

        // Iterate through compressed entries and fill gaps with 0
        for (const auto& entry : compressed) {
            // Fill gaps from current_idx to entry.index - 1 with 0
            while (current_idx < entry.index) {
                hash = mix(hash, 0ULL);
                current_idx++;
            }
            // Apply the accumulated value
            hash = mix(hash, entry.value);
            current_idx = entry.index + 1;
        }

        // Fill remaining gaps up to domain
        while (current_idx < domain) {
            hash = mix(hash, 0ULL);
            current_idx++;
        }
        
        final_hash = hash;
    }

    return final_hash;
}