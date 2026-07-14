#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t num_bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(num_bins, 0u);
    
    const std::size_t n = dna.size();
    const std::size_t kk = static_cast<std::size_t>(k);
    
    if (n >= kk && kk > 0) {
        static const unsigned char base_map[256] = {
            ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3,
        };
        
        uint32_t code = 0;
        for (std::size_t i = 0; i < kk; ++i) {
            code = (code << 2) | base_map[static_cast<unsigned char>(dna[i])];
        }
        dense[code]++;
        
        const uint32_t mask = (1u << (2 * k)) - 1;
        for (std::size_t i = kk; i < n; ++i) {
            code = ((code << 2) & mask) | base_map[static_cast<unsigned char>(dna[i])];
            dense[code]++;
        }
    }
    
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : dense) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    
    (void)iters;
    return hash;
}