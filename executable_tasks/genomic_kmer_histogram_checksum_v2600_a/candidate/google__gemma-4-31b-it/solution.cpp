#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast lookup table for DNA to 2-bit encoding
// A=0, C=1, G=2, T=3
static const uint8_t dna_map[256] = [] {
    uint8_t m[256] = {0};
    m['A'] = 0;
    m['C'] = 1;
    m['G'] = 2;
    m['T'] = 3;
    return m;
}();

inline uint64_t compute_checksum(const uint32_t* counts, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (dna.size() < static_cast<size_t>(k)) {
        // Handle case where dna is shorter than k
        size_t bins = 1ULL << (2 * k);
        std::vector<uint32_t> dense(bins, 0);
        uint64_t final_hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            final_hash = compute_checksum(dense.data(), bins);
        }
        return final_hash;
    }

    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins);
    const uint32_t mask = (1U << (2 * k)) - 1;
    
    uint64_t last_hash = 0;
    const char* data = dna.data();
    const size_t n = dna.size();

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t current_kmer = 0;
        // Initialize first k-mer
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | dna_map[static_cast<unsigned char>(data[i])];
        }
        dense[current_kmer]++;

        // Slide window
        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | dna_map[static_cast<unsigned char>(data[i])]) & mask;
            dense[current_kmer]++;
        }
        
        last_hash = compute_checksum(dense.data(), bins);
    }

    return last_hash;
}