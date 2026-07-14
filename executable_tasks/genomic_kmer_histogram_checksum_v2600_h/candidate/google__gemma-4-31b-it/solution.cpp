#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table for DNA bases to 2-bit values
// A=0, C=1, G=2, T=3
static const uint8_t base_to_bits[256] = []() {
    uint8_t table[256] = {0};
    table[(uint8_t)'C'] = 1;
    table[(uint8_t)'G'] = 2;
    table[(uint8_t)'T'] = 3;
    return table;
}();

inline uint64_t compute_checksum(const std::vector<uint32_t>& counts) {
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
    
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins);
    const size_t n = dna.size();
    const size_t k_size = static_cast<size_t>(k);
    
    if (n < k_size) {
        // If DNA is shorter than k, the histogram is all zeros
        // We still need to run the checksum iters times to match baseline behavior
        std::fill(dense.begin(), dense.end(), 0);
        uint64_t final_hash = 0;
        for (int i = 0; i < iters; ++i) {
            final_hash = compute_checksum(dense);
        }
        return final_hash;
    }

    const uint32_t mask = (1u << (2 * k)) - 1;
    uint64_t total_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0);
        
        uint32_t current_kmer = 0;
        // Initialize first k-mer
        for (size_t i = 0; i < k_size; ++i) {
            current_kmer = (current_kmer << 2) | base_to_bits[(uint8_t)dna[i]];
        }
        dense[current_kmer]++;

        // Slide window
        for (size_t i = k_size; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | base_to_bits[(uint8_t)dna[i]]) & mask;
            dense[current_kmer]++;
        }
        
        total_hash = compute_checksum(dense);
    }

    return total_hash;
}