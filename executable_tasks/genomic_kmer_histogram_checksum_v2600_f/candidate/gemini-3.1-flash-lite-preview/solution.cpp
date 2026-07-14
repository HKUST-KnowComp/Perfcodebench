#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

inline uint32_t char_to_bits(char c) {
    // A=00, C=01, G=10, T=11
    // Using a small lookup table or bit manipulation
    return (static_cast<uint32_t>(c >> 1) & 3u);
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
    const std::size_t n = dna.size();
    if (n < static_cast<std::size_t>(k)) return 0;

    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;

    const uint32_t mask = (1u << (2 * k)) - 1;
    
    // Pre-convert DNA to bits to avoid repeated char checks
    std::vector<uint32_t> dna_bits(n);
    for (std::size_t i = 0; i < n; ++i) {
        dna_bits[i] = char_to_bits(dna[i]);
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | dna_bits[i];
        }
        dense[current_kmer]++;

        for (std::size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | dna_bits[i]) & mask;
            dense[current_kmer]++;
        }
        
        hash = checksum_counts(dense);
    }
    return hash;
}