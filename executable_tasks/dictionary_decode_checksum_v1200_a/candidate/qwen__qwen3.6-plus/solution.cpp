#include "interface.h"
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    struct Entry {
        uint32_t len;
        uint8_t first;
    };
    
    std::vector<Entry> entries(dict.size());
    for (size_t i = 0; i < dict.size(); ++i) {
        const auto& s = dict[i];
        entries[i].len = static_cast<uint32_t>(s.size());
        entries[i].first = s.empty() ? 0 : static_cast<uint8_t>(s[0]);
    }

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    const Entry* entries_ptr = entries.data();
    const uint32_t* codes_ptr = codes.data();
    const uint32_t* codes_end = codes_ptr + codes.size();

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (const uint32_t* p = codes_ptr; p != codes_end; ++p) {
            const Entry& e = entries_ptr[*p];
            hash ^= e.len;
            hash *= FNV_PRIME;
            hash ^= e.first;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}