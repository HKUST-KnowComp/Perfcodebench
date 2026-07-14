#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

// Lookup table for base encoding: A=0, C=1, G=2, T=3
// Invalid chars map to 0 (same as A)
alignas(64) constexpr uint8_t BASE_ENCODE[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0, // A=65->0, C=67->1, G=71->2
    0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0, // T=84->3
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

inline uint64_t checksum_counts(const uint32_t* counts, size_t bins) {
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
    if (n < static_cast<size_t>(k) || k <= 0) {
        // Edge case: no k-mers possible
        const size_t bins = 1ULL << (2 * k);
        std::vector<uint32_t> dense(bins, 0u);
        return checksum_counts(dense.data(), bins);
    }
    
    const size_t bins = 1ULL << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    const size_t num_kmers = n - static_cast<size_t>(k) + 1;
    
    // Allocate histogram
    std::vector<uint32_t> dense(bins);
    
    const char* data = dna.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Clear histogram
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        // Encode first k-mer
        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | BASE_ENCODE[static_cast<unsigned char>(data[i])];
        }
        dense[code]++;
        
        // Rolling hash for remaining k-mers
        for (size_t i = 1; i < num_kmers; ++i) {
            // Shift out oldest base, shift in newest base
            code = ((code << 2) | BASE_ENCODE[static_cast<unsigned char>(data[i + k - 1])]) & mask;
            dense[code]++;
        }
        
        hash = checksum_counts(dense.data(), bins);
    }
    
    return hash;
}