#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1ULL << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    std::vector<uint32_t> dense(bins, 0u);
    
    uint8_t char_to_val[256] = {0};
    char_to_val['A'] = 0; char_to_val['C'] = 1; char_to_val['G'] = 2; char_to_val['T'] = 3;
    char_to_val['a'] = 0; char_to_val['c'] = 1; char_to_val['g'] = 2; char_to_val['t'] = 3;

    const char* data = dna.data();
    size_t n = dna.size();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t code = 0;
        size_t pre_len = (k > 0 && static_cast<size_t>(k) - 1 < n) ? static_cast<size_t>(k) - 1 : n;
        
        for (size_t i = 0; i < pre_len; ++i) {
            code = ((code << 2) & mask) | char_to_val[static_cast<unsigned char>(data[i])];
        }
        
        for (size_t i = pre_len; i < n; ++i) {
            code = ((code << 2) & mask) | char_to_val[static_cast<unsigned char>(data[i])];
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