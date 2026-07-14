#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint8_t char_to_code(char c) {
    switch (c) {
        case 'A': return 0u;
        case 'C': return 1u;
        case 'G': return 2u;
        case 'T': return 3u;
        default: return 0u;
    }
}

inline uint32_t encode_kmer_rolling(const std::string& dna, std::size_t start, int k) {
    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
        code = (code << 2) | char_to_code(dna[start + i]);
    }
    return code;
}

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        std::size_t n = dna.size();
        if (n < static_cast<std::size_t>(k)) {
            hash = checksum_counts(dense);
            continue;
        }
        
        uint32_t code = 0u;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | char_to_code(dna[i]);
        }
        dense[code]++;
        
        uint32_t mask = (1u << (2 * k)) - 1u;
        for (std::size_t i = k; i < n; ++i) {
            code = ((code << 2) | char_to_code(dna[i])) & mask;
            dense[code]++;
        }
        
        hash = checksum_counts(dense);
    }
    
    return hash;
}