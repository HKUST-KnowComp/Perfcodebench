#include "interface.h"
#include <vector>
#include <cstring>

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
    std::vector<uint64_t> sums(dict_size, 0);
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // clear sums vector quickly
        std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));

        // tight numeric gather-add
        for (size_t i = 0; i < n; ++i) {
            sums[codes[i]] += static_cast<uint64_t>(values[i]);
        }

        // compute checksum in dictionary order
        hash = 1469598103934665603ULL;
        for (size_t k = 0; k < dict_size; ++k) {
            hash = mix(hash, sums[k]);
        }
    }
    return hash;
}