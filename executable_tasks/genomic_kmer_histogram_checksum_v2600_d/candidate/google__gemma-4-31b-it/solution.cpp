#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins);
    
    // Precompute character mapping to avoid branching
    // A=0, C=1, G=2, T=3
    uint8_t map[256] = {0};
    map['A'] = 0;
    map['C'] = 1;
    map['G'] = 2;
    map['T'] = 3;

    const size_t n = dna.size();
    const size_t k_size = static_cast<size_t>(k);
    const uint32_t mask = (1u << (2 * k)) - 1;

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        if (n >= k_size) {
            uint32_t current_kmer = 0;
            // Initialize first k-mer
            for (size_t i = 0; i < k_size; ++i) {
                current_kmer = (current_kmer << 2) | map[static_cast<unsigned char>(dna[i])];
            }
            dense[current_kmer]++;

            // Slide window
            for (size_t i = 1; i <= n - k_size; ++i) {
                current_kmer = ((current_kmer << 2) | map[static_cast<unsigned char>(dna[i + k_size - 1])]) & mask;
                dense[current_kmer]++;
            }
        }

        // FNV-1a style checksum as per baseline
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}