#include "interface.h"

#include <cstring>
#include <string>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    uint64_t final_hash = 0;
    const size_t dict_size = dictionary.size();
    std::vector<uint64_t> sums(dict_size, 0);
    const size_t entry_count = codes.size();
    const uint16_t* __restrict__ codes_data = codes.data();
    const uint32_t* __restrict__ values_data = values.data();
    uint64_t* __restrict__ sums_data = sums.data();

    static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sums_data, 0, dict_size * sizeof(uint64_t));

        for (size_t i = 0; i < entry_count; ++i) {
            sums_data[codes_data[i]] += values_data[i];
        }

        final_hash = FNV_OFFSET;
        for (size_t i = 0; i < dict_size; ++i) {
            final_hash ^= sums_data[i];
            final_hash *= FNV_PRIME;
        }
    }

    return final_hash;
}