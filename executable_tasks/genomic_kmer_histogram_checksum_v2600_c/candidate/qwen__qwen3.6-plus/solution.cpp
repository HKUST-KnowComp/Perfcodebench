#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

inline uint8_t dna_to_bits(char c) {
    static const uint8_t lut[256] = {
        ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
    };
    return lut[static_cast<unsigned char>(c)];
}

uint64_t fnv1a_checksum(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t n = dna.size();
    const std::size_t bins = 1u << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        if (n >= static_cast<std::size_t>(k)) {
            uint32_t code = 0;
            // Initialize the first k-1 characters
            for (std::size_t i = 0; i < static_cast<std::size_t>(k) - 1; ++i) {
                code = (code << 2) | dna_to_bits(dna[i]);
            }
            
            // Slide window across the sequence
            for (std::size_t i = static_cast<std::size_t>(k) - 1; i < n; ++i) {
                code = ((code << 2) & mask) | dna_to_bits(dna[i]);
                dense[code]++;
            }
        }
        
        final_hash = fnv1a_checksum(dense);
    }
    
    return final_hash;
}