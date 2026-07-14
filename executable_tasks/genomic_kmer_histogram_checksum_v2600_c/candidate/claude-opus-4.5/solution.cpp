#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint32_t base_to_bits(char c) {
    // A=0, C=1, G=2, T=3
    // Using arithmetic: (c >> 1) & 3 gives: A(65)->0, C(67)->1, G(71)->3, T(84)->2
    // We need a lookup or formula that works correctly
    // A=65=0b1000001, C=67=0b1000011, G=71=0b1000111, T=84=0b1010100
    // (c & 6) >> 1: A->0, C->1, G->3, T->2 -- close but G and T swapped
    // Let's use a simple lookup table for speed
    static const uint32_t table[256] = {
        ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
    };
    return table[static_cast<unsigned char>(c)];
}

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t n = dna.size();
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    
    if (static_cast<std::size_t>(k) > n) {
        // No k-mers possible
        std::vector<uint32_t> dense(bins, 0u);
        return checksum_counts(dense.data(), bins);
    }
    
    std::vector<uint32_t> dense(bins);
    const char* data = dna.data();
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        // Encode first k-mer
        uint32_t kmer = 0;
        for (int i = 0; i < k; ++i) {
            kmer = (kmer << 2) | base_to_bits(data[i]);
        }
        ++dense[kmer];
        
        // Rolling hash for remaining k-mers
        const std::size_t num_kmers = n - static_cast<std::size_t>(k);
        for (std::size_t i = 0; i < num_kmers; ++i) {
            kmer = ((kmer << 2) | base_to_bits(data[i + k])) & mask;
            ++dense[kmer];
        }
        
        hash = checksum_counts(dense.data(), bins);
    }
    
    return hash;
}