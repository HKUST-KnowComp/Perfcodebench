#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table for DNA to 2-bit encoding
// A=0, C=1, G=2, T=3
static const uint8_t dna_map[256] = {
    ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3,
    ['a'] = 0, ['c'] = 1, ['g'] = 2, ['t'] = 3
};

uint64_t compute_checksum(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (k <= 0) return 0;
    
    const size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins);
    
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) {
        // If DNA is shorter than k, the histogram is all zeros
        // The checksum of a zeroed vector is constant
        std::fill(dense.begin(), dense.end(), 0u);
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = compute_checksum(dense);
        }
        return hash;
    }

    const uint32_t mask = bins - 1;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        // Initialize first k-mer
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | dna_map[static_cast<uint8_t>(dna[i])];
        }
        dense[current_kmer]++;

        // Slide window
        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | dna_map[static_cast<uint8_t>(dna[i])]) & mask;
            dense[current_kmer]++;
        }
        
        final_hash = compute_checksum(dense);
    }

    return final_hash;
}