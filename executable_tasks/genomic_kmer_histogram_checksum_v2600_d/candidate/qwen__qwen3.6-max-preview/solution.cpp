#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const uint32_t bins = 1u << (2 * k);
    const uint32_t mask = bins - 1;
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;
    
    const char* data = dna.data();
    const size_t n = dna.size();
    const size_t k_sz = static_cast<size_t>(k);
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t code = 0;
        size_t len = 0;
        
        for (size_t i = 0; i < n; ++i) {
            char c = data[i];
            uint32_t v = 0;
            if (c == 'C') v = 1;
            else if (c == 'G') v = 2;
            else if (c == 'T') v = 3;
            
            code = ((code << 2) | v) & mask;
            ++len;
            if (len >= k_sz) {
                ++dense[code];
            }
        }
        
        uint64_t h = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            h ^= static_cast<uint64_t>(count);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}