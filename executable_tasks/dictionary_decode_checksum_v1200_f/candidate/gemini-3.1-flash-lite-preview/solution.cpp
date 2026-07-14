#include "interface.h"
#include <vector>
#include <cstdint>

struct TokenMeta {
    uint64_t size;
    uint64_t first_byte;
};

uint64_t dictionary_decode_checksum(const std::vector<std::string>& dict, const std::vector<uint32_t>& codes, int iters) {
    const size_t dict_size = dict.size();
    std::vector<TokenMeta> meta(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        meta[i].size = dict[i].size();
        meta[i].first_byte = dict[i].empty() ? 0 : static_cast<unsigned char>(dict[i][0]);
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint32_t code : codes) {
            const TokenMeta& m = meta[code];
            hash ^= m.size;
            hash *= FNV_PRIME;
            hash ^= m.first_byte;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}