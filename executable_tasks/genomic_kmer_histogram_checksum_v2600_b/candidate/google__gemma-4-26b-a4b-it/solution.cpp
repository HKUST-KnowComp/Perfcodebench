#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast mapping for DNA bases to 2-bit representation
// A=0, C=1, G=2, T=3
inline uint32_t base_to_bits(char c) {
    switch (c) {
        case 'C': return 1u;
        case 'G': return 2u;
        case 'T': return 3u;
        default:  return 0u; // 'A' or other
    }
}

inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (k <= 0 || static_cast<size_t>(k) > dna.size()) {
        // Handle edge case where no k-mers exist
        const size_t bins = 1u << (2 * k);
        std::vector<uint32_t> dense(bins, 0u);
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_counts(dense);
        }
        return hash;
    }

    const size_t num_kmers = dna.size() - k + 1;
    const size_t bins = 1u << (2 * k);
    const uint32_t mask = (1u << (2 * k)) - 1;
    
    std::vector<uint32_t> dense(bins);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0u;
        // Initialize first k-mer
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | base_to_bits(dna[i]);
        }
        dense[current_kmer]++;

        // Slide the window
        for (size_t i = 1; i < num_kmers; ++i) {
            current_kmer = ((current_kmer << 2) | base_to_bits(dna[i + k - 1])) & mask;
            dense[current_kmer]++;
        }

        final_hash = checksum_counts(dense);
    }

    return final_hash;
}