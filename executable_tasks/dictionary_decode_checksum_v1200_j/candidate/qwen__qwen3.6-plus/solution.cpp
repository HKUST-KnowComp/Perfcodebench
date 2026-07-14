#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    struct Entry {
        uint32_t len;
        uint8_t first;
    };
    
    std::vector<Entry> lookup(dict.size());
    for (size_t i = 0; i < dict.size(); ++i) {
        lookup[i].len = static_cast<uint32_t>(dict[i].size());
        lookup[i].first = dict[i].empty() ? 0 : static_cast<uint8_t>(dict[i][0]);
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t code : codes) {
            const Entry& e = lookup[code];
            hash ^= e.len;
            hash *= 1099511628211ULL;
            hash ^= e.first;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}