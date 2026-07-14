#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>

namespace {
inline uint32_t char_to_val(char c) {
    // A=00, C=01, G=10, T=11
    // Using bitwise tricks: (c >> 1) & 3 works for A, C, G, T
    return (static_cast<uint32_t>(c) >> 1) & 3u;
}

uint64_t checksum_counts(const uint32_t* counts, size_t bins) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}
} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) return 0;
    
    const size_t bins = 1u << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    std::vector<uint32_t> dense(bins);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | char_to_val(dna[i]);
        }
        dense[current_kmer]++;

        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) & mask) | char_to_val(dna[i]);
            dense[current_kmer]++;
        }
        
        hash = checksum_counts(dense.data(), bins);
    }
    return hash;
}