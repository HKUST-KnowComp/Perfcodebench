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
    if (k <= 0 || dna.size() < static_cast<size_t>(k)) {
        // If k is invalid or dna too short, the histogram is all zeros
        // However, the problem implies k is valid for the DNA provided.
        // We follow the logic: if no kmers exist, dense is all 0s.
    }

    const size_t num_bins = 1u << (2 * k);
    const size_t n = dna.size();
    const size_t k_size = static_cast<size_t>(k);
    const uint32_t mask = (1u << (2 * k)) - 1;
    
    std::vector<uint32_t> dense(num_bins);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        if (n >= k_size) {
            uint32_t current_kmer = 0u;
            // Initialize first k-1 bases
            for (size_t i = 0; i < k_size - 1; ++i) {
                current_kmer = (current_kmer << 2) | base_to_bits(dna[i]);
            }

            // Slide the window
            for (size_t i = k_size - 1; i < n; ++i) {
                current_kmer = ((current_kmer << 2) | base_to_bits(dna[i])) & mask;
                dense[current_kmer]++;
            }
        }
        
        final_hash = checksum_counts(dense);
    }

    return final_hash;
}
