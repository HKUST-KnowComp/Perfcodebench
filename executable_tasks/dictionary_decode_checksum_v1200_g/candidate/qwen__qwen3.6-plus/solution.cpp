#include "interface.h"
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    struct Entry {
        size_t len;
        uint8_t first;
    };
    
    std::vector<Entry> info(dict.size());
    for (size_t i = 0; i < dict.size(); ++i) {
        const auto& s = dict[i];
        info[i].len = s.size();
        info[i].first = s.empty() ? 0 : static_cast<uint8_t>(static_cast<unsigned char>(s[0]));
    }
    
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint32_t code : codes) {
            const Entry& e = info[code];
            hash ^= e.len;
            hash *= FNV_PRIME;
            hash ^= e.first;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}