#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// FNV-1a constants for 64-bit hashing
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast mapping of DNA characters to 2-bit codes
// A=0, C=1, G=2, T=3. Other characters map to 0 (not expected in valid DNA).
static inline uint32_t char_to_code(char c) {
    static const uint8_t table[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P
        0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q, R, S, T, U, V, W, X, Y, Z
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    return table[static_cast<uint8_t>(c)];
}

}

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (dna.size() < static_cast<size_t>(k)) {
        // If DNA is shorter than k, all bins are 0.
        const size_t bins = 1ULL << (2 * k);
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < bins; ++i) {
            hash ^= 0ULL;
            hash *= FNV_PRIME;
        }
        return hash;
    }

    const size_t bins = 1ULL << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        // Initial k-mer
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | char_to_code(dna[i]);
        }
        dense[current_kmer]++;

        // Rolling k-mer processing
        const size_t n = dna.size();
        const char* data = dna.data();
        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | char_to_code(data[i])) & mask;
            dense[current_kmer]++;
        }

        // Compute FNV-1a checksum over the dense histogram
        uint64_t hash = FNV_OFFSET_BASIS;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}
