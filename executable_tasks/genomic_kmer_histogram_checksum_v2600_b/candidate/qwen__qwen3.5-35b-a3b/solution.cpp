#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace {

uint32_t char_to_code(char c) {
    if (c == 'A') return 0;
    else if (c == 'C') return 1;
    else if (c == 'G') return 2;
    else if (c == 'T') return 3;
    return 0;
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
    uint32_t mask = (1u << (2 * (k - 1))) - 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        if (dna.size() < static_cast<std::size_t>(k)) {
            hash = checksum_counts(dense);
            continue;
        }
        
        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | char_to_code(dna[i]);
        }
        dense[code]++;
        
        for (std::size_t start = 1; start + k <= dna.size(); ++start) {
            code = ((code & mask) << 2) | char_to_code(dna[start + k - 1]);
            dense[code]++;
        }
        
        hash = checksum_counts(dense);
    }
    return hash;
}