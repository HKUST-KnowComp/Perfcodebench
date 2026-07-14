#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (k <= 0) k = 1;
    const size_t bins = size_t(1) << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;
    
    const char* data = dna.data();
    const size_t n = dna.size();
    
    uint8_t cmap[256] = {0};
    cmap['C'] = 1;
    cmap['G'] = 2;
    cmap['T'] = 3;
    
    const size_t init_len = static_cast<size_t>(k) - 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t code = 0;
        size_t i = 0;
        
        // Build initial window of k-1 characters
        for (; i < init_len && i < n; ++i) {
            code = (code << 2) | cmap[static_cast<unsigned char>(data[i])];
        }
        
        // Slide window across the remaining characters
        for (; i < n; ++i) {
            code = ((code << 2) & mask) | cmap[static_cast<unsigned char>(data[i])];
            dense[code]++;
        }
        
        // FNV-1a checksum over the dense histogram
        uint64_t h = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            h ^= static_cast<uint64_t>(count);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}
