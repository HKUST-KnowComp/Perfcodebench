#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

inline uint32_t char_to_int(char c) {
    // A=0, C=1, G=2, T=3
    // Using a small lookup table or branchless logic for speed
    if (c == 'C') return 1;
    if (c == 'G') return 2;
    if (c == 'T') return 3;
    return 0;
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
    const std::size_t num_bins = 1u << (2 * k);
    const std::size_t n = dna.size();
    const std::size_t k_size = static_cast<std::size_t>(k);
    
    if (n < k_size) {
        // If DNA is shorter than k, all bins are 0
        std::vector<uint32_t> dense(num_bins, 0u);
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_counts(dense);
        }
        return hash;
    }

    std::vector<uint32_t> dense(num_bins);
    const uint32_t mask = static_cast<uint32_t>((1u << (2 * k)) - 1);
    uint64_t final_hash = 0;

    // Pre-convert DNA to integer representation to avoid branching in the loop
    std::vector<uint8_t> dna_ints(n);
    for (std::size_t i = 0; i < n; ++i) {
        char c = dna[i];
        if (c == 'C') dna_ints[i] = 1;
        else if (c == 'G') dna_ints[i] = 2;
        else if (c == 'T') dna_ints[i] = 3;
        else dna_ints[i] = 0;
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        // Initialize first k-1 characters
        for (std::size_t i = 0; i < k_size - 1; ++i) {
            current_kmer = (current_kmer << 2) | dna_ints[i];
        }

        // Slide the window
        for (std::size_t i = k_size - 1; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | dna_ints[i]) & mask;
            dense[current_kmer]++;
        }

        final_hash = checksum_counts(dense);
    }

    return final_hash;
}