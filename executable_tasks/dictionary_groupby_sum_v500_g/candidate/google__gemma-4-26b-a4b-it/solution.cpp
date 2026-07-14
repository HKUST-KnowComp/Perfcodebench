#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    
    if (dictionary.empty()) return 0;

    // Pre-allocate the aggregation buffer once to avoid repeated allocations.
    // Using the size of the dictionary as the bounds for the codes.
    std::vector<uint64_t> sums(dictionary.size());
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset sums for the current iteration
        std::fill(sums.begin(), sums.end(), 0ULL);

        // Direct aggregation using integer codes (O(N) complexity)
        // This avoids all string decoding and string-based hashing.
        const size_t n = codes.size();
        const uint16_t* c_ptr = codes.data();
        const uint32_t* v_ptr = values.data();
        
        for (size_t i = 0; i < n; ++i) {
            sums[c_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        // Calculate checksum in dictionary order
        final_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < dictionary.size(); ++i) {
            final_hash = mix(final_hash, sums[i]);
        }
    }

    return final_hash;
}