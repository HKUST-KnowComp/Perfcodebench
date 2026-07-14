#include "interface.h"
#include <vector>
#include <cstdint>

struct TokenMeta {
    uint64_t size;
    uint64_t first_byte;
};

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    const size_t n = dict.size();
    std::vector<TokenMeta> meta(n);
    for (size_t i = 0; i < n; ++i) {
        meta[i].size = dict[i].size();
        meta[i].first_byte = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
    }

    uint64_t hash = 0;
    const uint32_t* codes_ptr = codes.data();
    const size_t codes_size = codes.size();
    const TokenMeta* meta_ptr = meta.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < codes_size; ++i) {
            const TokenMeta& m = meta_ptr[codes_ptr[i]];
            
            hash ^= m.size;
            hash *= 1099511628211ULL;
            
            hash ^= m.first_byte;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}