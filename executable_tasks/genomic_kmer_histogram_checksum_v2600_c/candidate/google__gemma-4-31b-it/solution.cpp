#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table for DNA bases to 2-bit encoding
// A=0, C=1, G=2, T=3
static const uint8_t DNA_LOOKUP[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32-47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48-63
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80-95
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96-111 ('C' is 67)
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, // 112-127 ('G' is 71)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128-143
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144-159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160-175
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176-191
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192-207
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208-223
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224-239
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240-255
};

// Correcting the lookup table for 'T' (84) and 'A' (65)
// Since the static array above is a template, let's just use a helper function or a properly initialized array.
}

// Re-implementing with a simple helper to avoid static array initialization errors
inline uint32_t get_val(char c) {
    if (c == 'C') return 1u;
    if (c == 'G') return 2u;
    if (c == 'T') return 3u;
    return 0u;
}

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins);
    uint64_t final_hash = 0;
    
    const size_t n = dna.size();
    if (n < (size_t)k) {
        // Handle case where dna is shorter than k
        for (int iter = 0; iter < iters; ++iter) {
            std::fill(dense.begin(), dense.end(), 0u);
            uint64_t hash = 1469598103934665603ULL;
            for (uint32_t count : dense) {
                hash ^= static_cast<uint64_t>(count);
                hash *= 1099511628211ULL;
            }
            final_hash = hash;
        }
        return final_hash;
    }

    const uint32_t mask = (1u << (2 * k)) - 1;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | get_val(dna[i]);
        }
        dense[current_kmer]++;

        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | get_val(dna[i])) & mask;
            dense[current_kmer]++;
        }

        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}
