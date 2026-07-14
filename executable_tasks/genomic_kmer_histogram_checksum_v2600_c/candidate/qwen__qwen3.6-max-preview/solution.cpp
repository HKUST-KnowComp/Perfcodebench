#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const uint32_t bins = 1u << (2 * k);
    const uint32_t mask = bins - 1;
    std::vector<uint32_t> dense(bins);
    uint64_t hash = 0;
    
    const size_t n = dna.size();
    const char* s = dna.data();
    
    if (n < static_cast<size_t>(k)) {
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t h = 1469598103934665603ULL;
            for (uint32_t i = 0; i < bins; ++i) {
                h *= 1099511628211ULL;
            }
            hash = h;
        }
        return hash;
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            char c = s[i];
            uint32_t v = (c == 'C') ? 1 : (c == 'G') ? 2 : (c == 'T') ? 3 : 0;
            code = (code << 2) | v;
        }
        dense[code]++;
        
        for (size_t i = k; i < n; ++i) {
            char c = s[i];
            uint32_t v = (c == 'C') ? 1 : (c == 'G') ? 2 : (c == 'T') ? 3 : 0;
            code = ((code << 2) & mask) | v;
            dense[code]++;
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
