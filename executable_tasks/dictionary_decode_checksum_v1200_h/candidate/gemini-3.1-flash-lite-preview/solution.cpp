#include "interface.h"
#include <vector>
#include <cstdint>

struct Entry {
    uint64_t size;
    uint64_t first_byte;
};

uint64_t dictionary_decode_checksum(const std::vector<std::string>& dict, const std::vector<uint32_t>& codes, int iters) {
    const size_t dict_size = dict.size();
    std::vector<Entry> metadata(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        metadata[i].size = dict[i].size();
        metadata[i].first_byte = dict[i].empty() ? 0 : static_cast<unsigned char>(dict[i][0]);
    }

    uint64_t final_hash = 0;
    const uint32_t* codes_ptr = codes.data();
    const size_t num_codes = codes.size();
    const Entry* meta_ptr = metadata.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_codes; ++i) {
            const Entry& e = meta_ptr[codes_ptr[i]];
            
            hash ^= e.size;
            hash *= 1099511628211ULL;
            
            hash ^= e.first_byte;
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    return final_hash;
}