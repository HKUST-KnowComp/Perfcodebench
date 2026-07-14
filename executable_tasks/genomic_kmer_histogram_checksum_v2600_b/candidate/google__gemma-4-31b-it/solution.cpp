#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table for DNA characters to 2-bit encoding
// A=0, C=1, G=2, T=3
static const uint8_t dna_map[256] = []() {
    uint8_t m[256] = {0};
    m['A'] = 0;
    m['C'] = 1;
    m['G'] = 2;
    m['T'] = 3;
    return m;
}();

inline uint64_t compute_checksum(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (dna.empty() || k <= 0 || dna.size() < static_cast<size_t>(k)) {
        // Handle edge cases where no k-mers exist
        size_t bins = 1ULL << (2 * k);
        std::vector<uint32_t> dense(bins, 0);
        return iters > 0 ? compute_checksum(dense) : 0;
    }

    const size_t bins = 1ULL << (2 * k);
    const uint32_t mask = bins - 1;
    std::vector<uint32_t> dense(bins);
    
    uint64_t final_hash = 0;
    const size_t n = dna.size();
    const char* data = dna.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0);
        
        uint32_t current_kmer = 0;
        // Initialize first k-mer
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | dna_map[static_cast<uint8_t>(data[i])];
        }
        dense[current_kmer]++;

        // Slide window
        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | dna_map[static_cast<uint8_t>(data[i])]) & mask;
            dense[current_kmer]++;
        }
        
        final_hash = compute_checksum(dense);
    }

    return final_hash;
}