#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

// Computes (base^exp) mod 2^64 in O(log exp) time.
uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
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
        uint64_t h = FNV_OFFSET_BASIS;
        uint64_t skip_multiplier = power(FNV_PRIME, domain);
        h *= skip_multiplier;
        return h;
    }

    // Pre-aggregate indices and values into a sorted list of unique entries.
    // This is done once per function call.
    std::vector<Entry> entries;
    entries.reserve(indices.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] < domain) {
            entries.push_back({indices[i], static_cast<uint64_t>(values[i])});
        }
    }
    
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.index < b.index;
    });

    std::vector<Entry> aggregated;
    if (!entries.empty()) {
        aggregated.reserve(entries.size());
        aggregated.push_back(entries[0]);
        for (size_t i = 1; i < entries.size(); ++i) {
            if (entries[i].index == aggregated.back().index) {
                aggregated.back().value += entries[i].value;
            } else {
                aggregated.push_back(entries[i]);
            }
        }
    }

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET_BASIS;
        uint32_t current_pos = 0;

        for (const auto& entry : aggregated) {
            // Skip zeros between current_pos and entry.index
            if (entry.index > current_pos) {
                h *= power(FNV_PRIME, entry.index - current_pos);
            }
            // Mix the non-zero value
            h ^= entry.value;
            h *= FNV_PRIME;
            current_pos = entry.index + 1;
        }

        // Skip remaining zeros until the end of the domain
        if (domain > current_pos) {
            h *= power(FNV_PRIME, domain - current_pos);
        }
        final_hash = h;
    }

    return final_hash;
}
