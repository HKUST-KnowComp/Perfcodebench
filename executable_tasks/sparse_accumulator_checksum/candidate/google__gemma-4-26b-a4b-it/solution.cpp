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
        return hash;
    }

    // Pre-allocate and prepare entries for sorting
    std::vector<Entry> entries(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        entries[i] = {indices[i], static_cast<uint64_t>(values[i])};
    }

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Sort entries by index to allow linear accumulation and gap detection
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.index < b.index;
        });

        // Accumulate values for duplicate indices
        std::vector<Entry> merged;
        merged.reserve(entries.size());
        for (const auto& e : entries) {
            if (!merged.empty() && merged.back().index == e.index) {
                merged.back().value += e.value;
            } else {
                merged.push_back(e);
            }
        }

        uint64_t hash = 1469598103934665603ULL;
        uint32_t current_idx = 0;

        // Process the domain by jumping between non-zero indices
        for (const auto& entry : merged) {
            // Fill gaps with 0
            while (current_idx < entry.index) {
                hash = mix(hash, 0ULL);
                current_idx++;
            }
            // Process the non-zero entry
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
