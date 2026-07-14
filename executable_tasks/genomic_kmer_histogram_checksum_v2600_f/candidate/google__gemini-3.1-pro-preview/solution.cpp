#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

// FNV-1a constants for 64-bit hashing
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast DNA character to 2-bit encoding
// A=0, C=1, G=2, T=3
static inline uint32_t char_to_bits(char c) {
    // Using a simple lookup table approach for speed
    // 'A' (65) -> 0, 'C' (67) -> 1, 'G' (71) -> 2, 'T' (84) -> 3
    // This mapping matches the baseline's logic.
    static const uint8_t lut[256] = []() {
        uint8_t table[256] = {0};
        table[(uint8_t)'C'] = 1;
        table[(uint8_t)'G'] = 2;
        table[(uint8_t)'T'] = 3;
        return table;
    }();
    return lut[static_cast<uint8_t>(c)];
}

}

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (k <= 0) return 0;
    
    const std::size_t num_bins = 1ULL << (2 * k);
    const std::size_t dna_len = dna.size();
    const uint32_t mask = static_cast<uint32_t>(num_bins - 1);
    
    // Use a vector for the dense histogram to handle memory safely
    std::vector<uint32_t> dense(num_bins, 0u);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset histogram for each iteration
        std::fill(dense.begin(), dense.end(), 0u);

        if (dna_len >= static_cast<std::size_t>(k)) {
            uint32_t current_kmer = 0;
            
            // Initialize the first k-1 characters
            for (int i = 0; i < k - 1; ++i) {
                current_kmer = (current_kmer << 2) | char_to_bits(dna[i]);
            }

            // Sliding window for the rest of the sequence
            for (std::size_t i = static_cast<std::size_t>(k) - 1; i < dna_len; ++i) {
                current_kmer = ((current_kmer << 2) | char_to_bits(dna[i])) & mask;
                dense[current_kmer]++;
            }
        }

        // Calculate FNV-1a checksum over the dense histogram
        uint64_t hash = FNV_OFFSET_BASIS;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}
