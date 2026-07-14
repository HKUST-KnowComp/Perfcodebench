#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>

namespace {
inline uint32_t base_to_val(char c) {
    // A=00, C=01, G=10, T=11
    return (static_cast<uint32_t>(c >> 1) & 3u);
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
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) return 0;
    
    const size_t bins = 1u << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    std::vector<uint32_t> dense(bins);
    
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | base_to_val(dna[i]);
        }
        dense[current_kmer]++;
        
        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | base_to_val(dna[i])) & mask;
            dense[current_kmer]++;
        }
        
        final_hash = checksum_counts(dense);
    }
    
    return final_hash;
}