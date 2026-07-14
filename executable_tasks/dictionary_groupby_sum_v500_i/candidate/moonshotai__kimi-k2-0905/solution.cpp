#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
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

    // one-time reusable buffer for per-code sums
    std::vector<uint64_t> sums(dict_size, 0);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // clear sums quickly
        std::fill(sums.begin(), sums.end(), 0);

        // accumulate values by code (no strings, no hash table)
        for (size_t i = 0; i < n; ++i) {
            sums[static_cast<size_t>(codes[i])] += static_cast<uint64_t>(values[i]);
        }

        // compute dictionary-order checksum
        hash = 1469598103934665603ULL;
        for (size_t k = 0; k < dict_size; ++k) {
            hash = mix(hash, sums[k]);
        }
    }
    return hash;
}