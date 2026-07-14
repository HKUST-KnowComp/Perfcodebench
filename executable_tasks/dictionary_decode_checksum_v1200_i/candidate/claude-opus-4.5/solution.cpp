#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

struct DictEntry {
    uint64_t len;
    uint64_t first_byte;
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    // Pre-compute dictionary entry metadata
    const size_t dict_size = dict.size();
    std::vector<DictEntry> entries(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        entries[i].len = dict[i].size();
        entries[i].first_byte = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
    }
    
    const DictEntry* __restrict__ entry_data = entries.data();
    const uint32_t* __restrict__ code_data = codes.data();
    const size_t num_codes = codes.size();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        size_t i = 0;
        
        // Process with prefetching
        constexpr size_t PREFETCH_DIST = 16;
        
        for (; i + PREFETCH_DIST < num_codes; ++i) {
            __builtin_prefetch(&entry_data[code_data[i + PREFETCH_DIST]], 0, 0);
            const DictEntry& e = entry_data[code_data[i]];
            hash = mix(hash, e.len);
            hash = mix(hash, e.first_byte);
        }
        
        // Process remaining
        for (; i < num_codes; ++i) {
            const DictEntry& e = entry_data[code_data[i]];
            hash = mix(hash, e.len);
            hash = mix(hash, e.first_byte);
        }
    }
    
    return hash;
}