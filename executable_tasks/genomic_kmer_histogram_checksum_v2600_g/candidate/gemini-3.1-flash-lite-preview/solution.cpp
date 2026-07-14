#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t n = dna.size();
    if (n < static_cast<std::size_t>(k)) return 0;

    const std::size_t bins = 1u << (2 * k);
    const uint32_t mask = (1u << (2 * k)) - 1;
    std::vector<uint32_t> dense(bins);
    
    uint8_t map[256];
    map['A'] = 0; map['C'] = 1; map['G'] = 2; map['T'] = 3;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | map[static_cast<uint8_t>(dna[i])];
        }
        dense[current_kmer]++;

        for (std::size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | map[static_cast<uint8_t>(dna[i])]) & mask;
            dense[current_kmer]++;
        }

        uint64_t h = 1469598103934665603ULL;
        for (std::size_t i = 0; i < bins; ++i) {
            h ^= static_cast<uint64_t>(dense[i]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}