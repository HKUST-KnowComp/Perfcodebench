#include "interface.h"
#include <vector>
#include <cstdint>

struct Entry {
    uint64_t size;
    uint64_t first_byte;
};

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    const size_t dict_size = dict.size();
    std::vector<Entry> meta(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        const auto& s = dict[i];
        meta[i] = {static_cast<uint64_t>(s.size()), s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]))};
    }

    uint64_t hash = 0;
    const uint32_t* codes_ptr = codes.data();
    const size_t num_codes = codes.size();
    const Entry* meta_ptr = meta.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_codes; ++i) {
            const Entry& e = meta_ptr[codes_ptr[i]];
            hash ^= e.size;
            hash *= 1099511628211ULL;
            hash ^= e.first_byte;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}