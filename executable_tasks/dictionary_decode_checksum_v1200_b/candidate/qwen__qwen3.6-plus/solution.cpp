#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    struct Entry {
        uint64_t len;
        uint64_t first;
    };
    
    std::vector<Entry> table(dict.size());
    for (size_t i = 0; i < dict.size(); ++i) {
        const auto& s = dict[i];
        table[i].len = s.size();
        table[i].first = s.empty() ? 0ULL : static_cast<unsigned char>(s[0]);
    }
    
    const Entry* tbl = table.data();
    const uint32_t* cds = codes.data();
    const size_t n = codes.size();
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            const Entry& e = tbl[cds[i]];
            hash ^= e.len;
            hash *= 1099511628211ULL;
            hash ^= e.first;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}