#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast encoding of DNA characters to 2-bit representation
// A=0, C=1, G=2, T=3
inline uint32_t char_to_bits(char c) {
    if (c == 'C') return 1u;
    if (c == 'G') return 2u;
    if (c == 'T') return 3u;
    return 0u; // 'A' or other
}

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t num_bins = 1u << (2 * k);
    const std::size_t n = dna.size();
    const std::size_t k_size = static_cast<std::size_t>(k);
    
    if (n < k_size) {
        // If DNA is shorter than k, the histogram is all zeros
        std::vector<uint32_t> dense(num_bins, 0u);
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_counts(dense);
        }
        return hash;
    }

    std::vector<uint32_t> dense(num_bins);
    uint64_t final_hash = 0;
    const uint32_t mask = (k < 16) ? (1u << (2 * k)) - 1 : 0xFFFFFFFFu; 
    // Note: k is limited by the fact that 2*k must fit in uint32_t for the index
    // The problem implies k is small enough for 2*k to fit in uint32_t (max k=15 or 16 depending on bit width)
    // Given the return type and logic, k is likely <= 15.

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0u;
        // Initialize first k-1 characters
        for (std::size_t i = 0; i < k_size - 1; ++i) {
            current_kmer = (current_kmer << 2) | char_to_bits(dna[i]);
        }

        // Sliding window
        const uint32_t bit_mask = (1u << (2 * k)) - 1;
        for (std::size_t i = k_size - 1; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | char_to_bits(dna[i])) & bit_mask;
            dense[current_kmer]++;
        }

        final_hash = checksum_counts(dense);
    }

    return final_hash;
}
