#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table to map DNA characters to 2-bit values
// A=0, C=1, G=2, T=3
static const uint8_t dna_map[256] = [] {
    uint8_t m[256] = {0};
    m['A'] = 0;
    m['C'] = 1;
    m['G'] = 2;
    m['T'] = 3;
    return m;
}();

inline uint64_t compute_fnv1a(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (dna.size() < static_cast<size_t>(k)) {
        // Handle case where dna is shorter than k
        const size_t bins = 1u << (2 * k);
        std::vector<uint32_t> dense(bins, 0);
        uint64_t final_hash = 0;
        for (int i = 0; i < iters; ++i) {
            final_hash = compute_fnv1a(dense);
        }
        return final_hash;
    }

    const size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins);
    const uint32_t mask = (1u << (2 * k)) - 1;
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        // Initialize first k-mer
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | dna_map[static_cast<unsigned char>(dna[i])];
        }
        dense[current_kmer]++;

        // Slide window
        for (size_t i = 1; i <= dna.size() - k; ++i) {
            current_kmer = ((current_kmer << 2) | dna_map[static_cast<unsigned char>(dna[i + k - 1])]) & mask;
            dense[current_kmer]++;
        }
        
        last_hash = compute_fnv1a(dense);
    }

    return last_hash;
}