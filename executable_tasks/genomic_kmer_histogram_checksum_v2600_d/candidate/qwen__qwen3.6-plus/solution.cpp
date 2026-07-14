#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins, 0);
    uint64_t hash = 0;
    const uint32_t mask = (1u << (2 * k)) - 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0);
        uint32_t code = 0;
        const size_t n = dna.size();
        for (size_t i = 0; i < n; ++i) {
            uint8_t c = static_cast<uint8_t>(dna[i]);
            uint32_t v = 0;
            if (c == 'C') v = 1;
            else if (c == 'G') v = 2;
            else if (c == 'T') v = 3;
            
            code = ((code << 2) & mask) | v;
            if (i >= static_cast<size_t>(k) - 1) {
                dense[code]++;
            }
        }
        
        hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}