#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

alignas(64) constexpr uint8_t BASE_TO_CODE[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0, // A=0, C=1, G=2
    0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0, // T=3
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
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    const std::size_t n = dna.size();
    const std::size_t kk = static_cast<std::size_t>(k);
    
    if (n < kk) {
        std::vector<uint32_t> dense(bins, 0u);
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_counts(dense.data(), bins);
        }
        return hash;
    }
    
    std::vector<uint32_t> dense(bins);
    const char* data = dna.data();
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        // Build initial k-mer code
        uint32_t code = 0;
        for (std::size_t i = 0; i < kk; ++i) {
            code = (code << 2) | BASE_TO_CODE[static_cast<unsigned char>(data[i])];
        }
        ++dense[code];
        
        // Rolling hash for remaining k-mers
        for (std::size_t i = kk; i < n; ++i) {
            code = ((code << 2) | BASE_TO_CODE[static_cast<unsigned char>(data[i])]) & mask;
            ++dense[code];
        }
        
        hash = checksum_counts(dense.data(), bins);
    }
    
    return hash;
}