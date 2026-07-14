#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

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
    int iters)
{
    const size_t n = codes.size();
    const size_t dict_size = dictionary.size();

    // Pre-compute integer keys once; avoids repeated string lookups.
    std::vector<uint16_t> keys(n);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = codes[i];
    }

    // Accumulator array indexed by dictionary position.
    std::vector<uint64_t> sums(dict_size, 0);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear accumulators.
        std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));

        // Single pass aggregation.
        for (size_t i = 0; i < n; ++i) {
            sums[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        // Compute checksum in dictionary order.
        hash = 1469598103934665603ULL;
        for (size_t j = 0; j < dict_size; ++j) {
            hash = mix(hash, sums[j]);
        }
    }
    return hash;
}